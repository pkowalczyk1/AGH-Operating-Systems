#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define REINDEER_CNT 9
#define ELF_CNT 10
#define MAX_ELVES_WAIT 3
#define MAX_GIFTS 3

int waiting_elves = 0;
int waiting_reindeers = 0;
int elves_ids[MAX_ELVES_WAIT];

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_wait_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_wait_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_wait_cond = PTHREAD_COND_INITIALIZER;

void* santa_loop(void *arg) {
    srand(time(NULL));
    int delivered = 0;
    while (delivered != MAX_GIFTS) {
        pthread_mutex_lock(&santa_mutex);
        while (waiting_elves != MAX_ELVES_WAIT && waiting_reindeers != REINDEER_CNT) {
            pthread_cond_wait(&santa_cond, &santa_mutex);
        }
        pthread_mutex_unlock(&santa_mutex);

        printf("Mikołaj: budzę się\n");
        pthread_mutex_lock(&reindeer_mutex);
        if (waiting_reindeers == REINDEER_CNT) {
            printf("Mikołaj: dostarczam zabawki\n");
            usleep(2000000 + (rand() % 2000000));
            delivered++;
            pthread_mutex_lock(&reindeer_wait_mutex);
            waiting_reindeers = 0;
            pthread_cond_broadcast(&reindeer_cond);
            pthread_mutex_unlock(&reindeer_wait_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);

        pthread_mutex_lock(&elves_mutex);
        if (waiting_elves == MAX_ELVES_WAIT) {
            pthread_mutex_lock(&elves_wait_mutex);
            printf("Mikołaj: rozwiązuję problemy elfów: %d %d %d\n", elves_ids[0], elves_ids[1], elves_ids[2]);
            usleep(1000000 + (rand() % 1000000));
            waiting_elves = 0;
            pthread_cond_broadcast(&elves_wait_cond);
            pthread_mutex_unlock(&elves_wait_mutex);
        }
        pthread_mutex_unlock(&elves_mutex);

        printf("Mikołaj: zasypiam\n");
    }

    exit(0);
}

void* elf_loop(void *arg) {
    int id = *(int *)arg;
    srand(id);

    while (1) {
        usleep(2000000 + (rand() % 3000000));
        pthread_mutex_lock(&elves_wait_mutex);
        while (waiting_elves == MAX_ELVES_WAIT) {
            printf("Elf: czeka na powrót elfów, %d\n", id);
            pthread_cond_wait(&elves_wait_cond, &elves_wait_mutex);
        }
        pthread_mutex_unlock(&elves_wait_mutex);

        pthread_mutex_lock(&elves_mutex);
        if (waiting_elves < MAX_ELVES_WAIT) {
            elves_ids[waiting_elves] = id;
            waiting_elves++;
            printf("Elf: czeka %d elfów na Mikołaja, %d\n", waiting_elves, id);
            if (waiting_elves == MAX_ELVES_WAIT) {
                printf("Elf: wybudzam mikołaja, %d\n", id);
                pthread_mutex_lock(&santa_mutex);
                pthread_cond_broadcast(&santa_cond);
                pthread_mutex_unlock(&santa_mutex);
            }
        }
        pthread_mutex_unlock(&elves_mutex);
    }
}

void* reindeer_loop(void *arg) {
    int id = *(int *)arg;
    srand(id);

    while (1) {
        pthread_mutex_lock(&reindeer_wait_mutex);
        while (waiting_reindeers != 0) {
            pthread_cond_wait(&reindeer_cond, &reindeer_wait_mutex);
        }
        pthread_mutex_unlock(&reindeer_wait_mutex);

        usleep(5000000 + (rand() % 5000000));

        pthread_mutex_lock(&reindeer_mutex);
        waiting_reindeers++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", waiting_reindeers, id);
        if (waiting_reindeers == REINDEER_CNT) {
            printf("Renifer: wybudza Mikołaja, %d\n", id);
            pthread_mutex_lock(&santa_mutex);
            pthread_cond_broadcast(&santa_cond);
            pthread_mutex_unlock(&santa_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);
        usleep(2000000 + (rand() % 2000000));
    }
}

int main() {
    pthread_t reindeer_threads[REINDEER_CNT];
    pthread_t elves_threads[ELF_CNT];
    pthread_t santa_thread;

    int reindeer_ids[REINDEER_CNT];
    int elf_ids[ELF_CNT];

    pthread_create(&santa_thread, NULL, santa_loop, NULL);
    for (int i = 0; i < REINDEER_CNT; i++) {
        reindeer_ids[i] = i;
        pthread_create(&reindeer_threads[i], NULL, reindeer_loop, &reindeer_ids[i]);
    }

    for (int i = 0; i < ELF_CNT; i++) {
        elf_ids[i] = i;
        pthread_create(&elves_threads[i], NULL, elf_loop, &elf_ids[i]);
    }

    for (int i = 0; i < REINDEER_CNT; i++) {
        pthread_join(reindeer_threads[i], NULL);
    }

    for (int i = 0; i < ELF_CNT; i++) {
        pthread_join(elves_threads[i], NULL);
    }

    return 0;
}
