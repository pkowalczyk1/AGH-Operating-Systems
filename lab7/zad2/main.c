#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <wait.h>
#include "defines.h"

Oven *oven;
Table *table;
sem_t *oven_sem_desc;
sem_t *table_sem_desc;
int mem_desc;
int children_cnt;
pid_t *children;

void end_main(int sig) {
    for (pid_t i = 0; i < children_cnt; i++) {
        kill(children[i], SIGINT);
    }
}

void handle_int(int sig) {
    exit(0);
}

sem_t* open_sem(char *name) {
    sem_t *sem_desc = sem_open(name, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if (sem_desc == SEM_FAILED) {
        sem_desc = sem_open(name, 0);
        if (sem_desc != SEM_FAILED) {
            sem_close(sem_desc);
            sem_unlink(name);
        }

        sem_desc = sem_open(name, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    }

    return sem_desc;
}

int open_mem(char *name) {
    int mem_desc = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (mem_desc == -1) {
        mem_desc = shm_open(name, 0, 0666);
        if (mem_desc != -1) {
            shm_unlink(name);
        }
        mem_desc = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
    }

    ftruncate(mem_desc, sizeof(Oven) + sizeof(Table));

    return mem_desc;
}

int get_timestamp() {
    return (int) time(NULL);
}

void cook_work() {
    srand(time(NULL));
    while (1) {
        int pizza_type = rand() % 10;
        int sleep_time = 100000 + (rand() % 1000000);
        printf("(%d %d) Przygotowuje pizze %d\n", getpid(), get_timestamp(), pizza_type);
        usleep(sleep_time);
        int placed_in_oven = 0;
        int placed_at = -1;
        while (!placed_in_oven) {
            sem_wait(oven_sem_desc);
            if (oven->places[oven->next_free] == -1) {
                placed_in_oven = 1;
                placed_at = oven->next_free;
                oven->places[oven->next_free] = pizza_type;
                oven->next_free++;
                oven->next_free %= PIZZA_MAX;
                oven->pizza_cnt++;
                printf("(%d %d) Dodalem pizze: %d. Liczba pizz w piekarniku: %d\n", getpid(), get_timestamp(), pizza_type, oven->pizza_cnt);
            }

            sem_post(oven_sem_desc);
        }

        sleep_time = 4000000 + (rand() % 1000000);
        usleep(sleep_time);
        int placed_on_table = 0;
        int oven_pizza_cnt;
        while (!placed_on_table) {
            sem_wait(table_sem_desc);
            if (table->places[table->next_free] == -1) {
                placed_on_table = 1;
                sem_wait(oven_sem_desc);
                oven->places[placed_at] = -1;
                oven->pizza_cnt--;
                oven_pizza_cnt = oven->pizza_cnt;
                sem_post(oven_sem_desc);

                table->places[table->next_free] = pizza_type;
                table->next_free++;
                table->next_free %= PIZZA_MAX;
                table->pizza_cnt++;
                printf("(%d %d) Wyjmuje pizze: %d. Lizba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), get_timestamp(), pizza_type, oven_pizza_cnt, table->pizza_cnt);
            }

            sem_post(table_sem_desc);
        }
    }
}

void driver_work() {
    srand(time(NULL));
    while (1) {
        sem_wait(table_sem_desc);
        int i = 0;
        while (i < PIZZA_MAX && table->places[i] == -1) {
            i++;
        }

        int pizza_type = -1;
        if (i != PIZZA_MAX) {
            pizza_type = table->places[i];
            table->places[i] = -1;
            table->pizza_cnt--;
            printf("(%d %d) Pobieram pizze: %d. Liczba pizz na stole: %d\n", getpid(), get_timestamp(), pizza_type, table->pizza_cnt);
        }

        sem_post(table_sem_desc);
        if (i != PIZZA_MAX) {
            int sleep_time = 4000000 + (rand() % 1000000);
            usleep(sleep_time);
            printf("(%d %d) Dostarczam pizze: %d\n", getpid(), get_timestamp(), pizza_type);
            sleep_time = 4000000 + (rand() % 1000000);
            usleep(sleep_time);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Too few arguments\n");
        return -1;
    }

    int cook_cnt = atoi(argv[1]);
    int driver_cnt = atoi(argv[2]);
    children_cnt = cook_cnt + driver_cnt;
    children = calloc(children_cnt, sizeof(pid_t));

    oven_sem_desc = open_sem(OVEN_SEM_NAME);
    table_sem_desc = open_sem(TABLE_SEM_NAME);
    mem_desc = open_mem(MEM_NAME);

    oven = (Oven *)mmap(NULL, sizeof(Oven) + sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, mem_desc, 0);
    table = (Table *) ((void *)oven + sizeof(Oven));
    oven->pizza_cnt = 0;
    table->pizza_cnt = 0;
    oven->next_free = 0;
    table->next_free = 0;
    for (int i = 0; i < PIZZA_MAX; i++) {
        oven->places[i] = -1;
        table->places[i] = -1;
    }

    int child_index = 0;
    for (int i = 0; i < cook_cnt; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            signal(SIGINT, handle_int);
            cook_work();
            return 0;
        }
        else {
            children[child_index] = pid;
            child_index++;
        }
    }

    for (int i = 0; i < driver_cnt; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            signal(SIGINT, handle_int);
            driver_work();
            return 0;
        }
        else {
            children[child_index] = pid;
            child_index++;
        }
    }

    signal(SIGINT, end_main);

    for (int i = 0; i < children_cnt; i++) {
        waitpid(children[i], NULL, 0);
    }

    free(children);
    munmap(oven, sizeof(Oven) + sizeof(Table));
    shm_unlink(MEM_NAME);
    sem_close(oven_sem_desc);
    sem_close(table_sem_desc);
    sem_unlink(TABLE_SEM_NAME);
    sem_unlink(OVEN_SEM_NAME);

    return 0;
}
