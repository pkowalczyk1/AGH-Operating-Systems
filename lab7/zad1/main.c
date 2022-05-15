#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <wait.h>
#include "defines.h"

Oven *oven;
Table *table;
int sem_desc;
int mem_desc;
int children_cnt;
pid_t *children;

void end_main(int sig) {
    for (pid_t i = 0; i < children_cnt; i++) {
        kill(children[i], SIGINT);
    }
}

void handle_int(int sig) {
    shmdt(oven);
    shmdt(table);
    exit(0);
}

int open_sem() {
    int sem_desc = semget(SEM_KEY, 2, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_desc == -1) {
        sem_desc = semget(SEM_KEY, 2, 0666);
        if (sem_desc != -1) {
            semctl(sem_desc, 2, IPC_RMID);
            sem_desc = semget(SEM_KEY, 2, 0666);
        }
    }

    return sem_desc;
}

int open_mem() {
    int mem_desc = shmget(MEM_KEY, sizeof(Oven) + sizeof(Table), IPC_CREAT | IPC_EXCL | 0666);
    if (mem_desc == -1) {
        mem_desc = shmget(MEM_KEY, sizeof(Oven) + sizeof(Table), 0666);
        if (mem_desc != -1) {
            shmctl(mem_desc, IPC_RMID, NULL);
            mem_desc = shmget(MEM_KEY, sizeof(Oven) + sizeof(Table), 0666);
        }
    }

    return mem_desc;
}

int get_timestamp() {
    return (int) time(NULL);
}

void cook_work() {
    srand(time(NULL));
    while (1) {
        struct sembuf buf = {
                .sem_op = -1,
                .sem_num = OVEN_SEM_INDEX,
                .sem_flg = 0,
        };
        int pizza_type = rand() % 10;
        int sleep_time = 100000 + (rand() % 1000000);
        printf("(%d %d) Przygotowuje pizze %d\n", getpid(), get_timestamp(), pizza_type);
        usleep(sleep_time);
        int placed_in_oven = 0;
        int placed_at = -1;
        while (!placed_in_oven) {
            semop(sem_desc, &buf, 1);
            if (oven->places[oven->next_free] == -1) {
                placed_in_oven = 1;
                placed_at = oven->next_free;
                oven->places[oven->next_free] = pizza_type;
                oven->next_free++;
                oven->next_free %= PIZZA_MAX;
                oven->pizza_cnt++;
                printf("(%d %d) Dodalem pizze: %d. Liczba pizz w piekarniku: %d\n", getpid(), get_timestamp(), pizza_type, oven->pizza_cnt);
            }

            buf.sem_op = 1;
            semop(sem_desc, &buf, 1);
        }

        sleep_time = 4000000 + (rand() % 1000000);
        usleep(sleep_time);
        int placed_on_table = 0;
        int oven_pizza_cnt;
        while (!placed_on_table) {
            buf.sem_op = -1;
            buf.sem_num = TABLE_SEM_INDEX;
            semop(sem_desc, &buf, 1);
            if (table->places[table->next_free] == -1) {
                placed_on_table = 1;
                buf.sem_num = OVEN_SEM_INDEX;
                semop(sem_desc, &buf, 1);
                oven->places[placed_at] = -1;
                oven->pizza_cnt--;
                oven_pizza_cnt = oven->pizza_cnt;
                buf.sem_op = 1;
                semop(sem_desc, &buf, 1);

                table->places[table->next_free] = pizza_type;
                table->next_free++;
                table->next_free %= PIZZA_MAX;
                table->pizza_cnt++;
                printf("(%d %d) Wyjmuje pizze: %d. Lizba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), get_timestamp(), pizza_type, oven_pizza_cnt, table->pizza_cnt);
            }

            buf.sem_op = 1;
            buf.sem_num = TABLE_SEM_INDEX;
            semop(sem_desc, &buf, 1);
        }
    }
}

void driver_work() {
    srand(time(NULL));
    while (1) {
        struct sembuf buf = {
                .sem_op = -1,
                .sem_num = TABLE_SEM_INDEX,
                .sem_flg = 0,
        };
        semop(sem_desc, &buf, 1);
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

        buf.sem_op = 1;
        semop(sem_desc, &buf, 1);
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

    sem_desc = open_sem();
    mem_desc = open_mem();
    senum options;
    options.val = 1;
    semctl(sem_desc, OVEN_SEM_INDEX, SETVAL, options);
    semctl(sem_desc, TABLE_SEM_INDEX, SETVAL, options);

    oven = shmat(mem_desc, NULL, 0);
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

    semctl(sem_desc, 0, IPC_RMID);
    shmdt(oven);
    shmdt(table);
    shmctl(mem_desc, IPC_RMID, NULL);

    return 0;
}
