#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/file.h>

int count_digits(int n) {
    int cnt = 0;
    while (n > 0) {
        cnt++;
        n /= 10;
    }

    return cnt;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 5) {
        fprintf(stderr, "Too few arguments");
    }

    int line_number = atoi(argv[2]);
    int read_size = atoi(argv[4]);

    FILE *fifo = fopen(argv[1], "w");
    FILE *input_file = fopen(argv[3], "r");

    int line_number_digits = count_digits(line_number);
    char *number_buffer = calloc(line_number_digits + 2, sizeof(char));
    sprintf(number_buffer, "%d:", line_number);
    char *read_buffer = calloc(read_size, sizeof(char));
    char *write_buffer = calloc(read_size + line_number + 2, sizeof(char));

    int read;
    while ((read = fread(read_buffer, sizeof(char), read_size, input_file)) > 0) {
        for (int i = read; i < read_size; i++) {
            read_buffer[i] = 0;
        }
        sprintf(write_buffer, "%s%s", number_buffer, read_buffer);
        fwrite(write_buffer, sizeof(char), line_number_digits + read_size + 1, fifo);
        int delay = (rand() % 18) + 3;
        usleep(delay * 10000);
        fseek(fifo, 0, SEEK_SET);
    }

    free(number_buffer);
    free(read_buffer);
    free(write_buffer);
    fclose(fifo);

    return 0;
}