#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

void write_part(FILE *file, char *part, int line_number) {
    int total_lines = 1;
    int fd = fileno(file);
    char c;
    int write_spot = -1;
    flock(fd, LOCK_EX);
    while (fread(&c, sizeof(char), 1, file) > 0) {
        if ((c == '\n' || c == '\0') && line_number == total_lines) {
            write_spot = ftell(file) - 1;
        }
        if (c == '\n') {
            total_lines++;
        }
    }

    int file_size = ftell(file);
    if (write_spot == -1) {
        while (total_lines != line_number) {
            fwrite("\n", sizeof(char), 1, file);
            total_lines++;
        }
        fwrite(part, sizeof(char), strlen(part), file);
    }
    else{
        fseek(file, write_spot, SEEK_SET);
        char *rest_of_file = calloc(sizeof(char), file_size - write_spot);
        fread(rest_of_file, sizeof(char), file_size - write_spot, file);
        fseek(file, write_spot, SEEK_SET);
        fwrite(part, sizeof(char), strlen(part), file);
        fwrite(rest_of_file, sizeof(char), file_size - write_spot, file);
        free(rest_of_file);
    }
    fseek(file, 0, SEEK_SET);
    flock(fd, LOCK_UN);
}

int read_data(FILE *file, int read_size, char *buffer, int *line_number) {
    char c;
    *line_number = 0;
    int read = fread(&c, sizeof(char), 1, file);
    while (read > 0) {
        if (c == ':') {
            break;
        }
        *line_number = 10 * (*line_number) + (c - '0');
        read = fread(&c, sizeof(char), 1, file);
    }

    return fread(buffer, sizeof(char), read_size, file);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Too few arguments");
    }

    FILE *fifo = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w+r");
    int read_size = atoi(argv[3]);
    char *read_buffer = calloc(read_size + 1, sizeof(char));
    int line_number;
    int read = read_data(fifo, read_size, read_buffer, &line_number);
    while (read > 0) {
        write_part(output_file, read_buffer, line_number);
        read = read_data(fifo, read_size, read_buffer, &line_number);
    }

    fclose(fifo);
    fclose(output_file);
    free(read_buffer);

    return 0;
}