#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

clock_t start_time, end_time;
struct tms start_cpu;
struct tms end_cpu;

void start_timer() {
    start_time = times(&start_cpu);
}

void end_timer(FILE *file) {
    int clock = sysconf(_SC_CLK_TCK);
    end_time = times(&end_cpu);
    fprintf(file, "%s:\nReal: %f, User: %f, System: %f\n",
            "Library functions",
            (double) (end_time - start_time) / clock,
            (double) (end_cpu.tms_utime - start_cpu.tms_utime) / clock,
            (double) (end_cpu.tms_stime - start_cpu.tms_stime) / clock);
}

int main(int argc, char *argv[]) {
    char source_name[256];
    char dest_name[256];

    FILE *report = fopen("pomiar_zad_1.txt", "a");

    if (argc < 3) {
        printf("Ścieżka do pliku kopiowanego: ");
        scanf("%s", source_name);
        printf("Ścieżka do pliku docelowego: ");
        scanf("%s", dest_name);
    }
    else {
        strcpy(source_name, argv[1]);
        strcpy(dest_name, argv[2]);
    }

    FILE *source = fopen(source_name, "r");
    FILE *dest = fopen(dest_name, "w");

    start_timer();

    char curr = fgetc(source);
    int size = 0;
    int is_blank = 1;
    while (curr != EOF) {
        size++;
        if (curr != ' ' && curr != '\t' && curr != '\n' && curr != '\r' && curr != '\0') {
            is_blank = 0;
        }
        if ((curr == '\r' || curr == '\n' || curr == '\0') && is_blank != 1) {
            fseek(source, -1 * size, 1);
            char *buf = calloc(size, sizeof(char));
            fread(buf, sizeof(char), size, source);
            fwrite(buf, sizeof(char), size, dest);
            free(buf);
            size = 0;
            is_blank = 1;
        }
        else if (curr == '\n' || curr == '\0' || curr == '\r') {
            size = 0;
            is_blank = 1;
        }
        curr = fgetc(source);
    }

    if (is_blank != 1) {
        fseek(source, -1 * size, 1);
        char *buf = calloc(size, sizeof(char));
        fread(buf, sizeof(char), size, source);
        fwrite(buf, sizeof(char), size, dest);
        free(buf);
    }

    fclose(source);
    fclose(dest);

    end_timer(report);
    fclose(report);

    return 0;
}