#ifndef LAB7_DEFINES_H
#define LAB7_DEFINES_H

#define PIZZA_MAX 5
#define OVEN_SEM_NAME "/oven_sem"
#define TABLE_SEM_NAME "/table_sem"
#define MEM_NAME "/shared_mem"

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

#endif //LAB7_DEFINES_H
