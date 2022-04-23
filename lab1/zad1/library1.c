#include "library1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **array = NULL;
unsigned int array_size = 0;
char *tmp_file = "tmp.txt";

int create_table(unsigned int size) {
    if (array != NULL) {
        fprintf(stderr, "Table is already initialized");
        return -1;
    }

    array = calloc(size, sizeof(char *));
    array_size = size;
    return 0;
}

unsigned int get_size(FILE *f) {
    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    rewind(f);
    return size;
}

int save_buf_to_array(char *buffer) {
    int i = 0;
    while (i < array_size && array[i] != NULL) {
        i++;
    }

    if (i == array_size) {
        fprintf(stderr, "Array is full");
        return -1;
    }

    array[i] = buffer;
    return i;
}

int save_tmp_to_array() {
    FILE *file = fopen(tmp_file, "r");
    if (!file) {
        fprintf(stderr, "Couldn't open file");
        return -1;
    }

    unsigned int size = get_size(file);
    char *buffer = calloc(size, sizeof(char));
    fread(buffer, sizeof(char), size, file);
    fclose(file);
    remove_tmp();
    return save_buf_to_array(buffer);
}

int remove_block(int index) {
    if (index >= array_size) {
        fprintf(stderr, "Index outside of array");
        return -1;
    }

    free(array[index]);
    array[index] = NULL;
    return 0;
}

void print_array() {
    for (int i = 0; i < array_size; i++) {
        printf("%s", array[i]);
        printf("\n");
    }
}

void free_array() {
    for (int i = 0; i < array_size; i++) {
        remove_block(i);
    }

    free(array);
}

int wc_files(char *files) {
    char buf[1000];
    snprintf(buf, sizeof(buf), "wc %s >> %s", files, tmp_file);
    int status = system(buf);
    if (status != 0) {
        return status;
    }

    return 0;
}

void remove_tmp() {
    remove(tmp_file);
}
