#ifndef LAB7_DEFINES_H
#define LAB7_DEFINES_H

#define SEM_KEY 843925
#define MEM_KEY 903482
#define PIZZA_MAX 5
#define TABLE_SEM_INDEX 0
#define OVEN_SEM_INDEX 1

typedef struct {
    int places[PIZZA_MAX];
    int next_free;
    int pizza_cnt;
} Oven;

typedef struct {
    int places[PIZZA_MAX];
    int next_free;
    int pizza_cnt;
} Table;

typedef union {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
} senum;

#endif //LAB7_DEFINES_H
