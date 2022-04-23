#include <stdio.h>
#include <string.h>
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
    int total = 0;
    int lines = 0;
    char file_name [256];

    if (argc < 3) {
        fprintf(stderr, "Too few arguments");
        return -1;
    }

    char c = argv[1][0];
    strcpy(file_name, argv[2]);

    FILE *report = fopen("pomiar_zad_2.txt", "a");

    start_timer();
    FILE *file = fopen(file_name, "r");
    char curr = fgetc(file);
    int found = 0;
    while (curr != EOF) {
        if (curr == c) {
            total++;
            found = 1;
        }
        if (curr == '\n') {
            if (found == 1) {
                lines++;
            }
            found = 0;
        }
        curr = fgetc(file);
    }

    if (found == 1) {
        lines++;
    }

    printf("total: %d; lines: %d\n", total, lines);
    fclose(file);

    end_timer(report);
    fclose(report);

    return 0;
}