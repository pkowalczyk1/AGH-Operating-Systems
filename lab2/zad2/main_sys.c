#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>

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
            "System functions",
            (double) (end_time - start_time) / clock,
            (double) (end_cpu.tms_utime - start_cpu.tms_utime) / clock,
            (double) (end_cpu.tms_stime - start_cpu.tms_stime) / clock);
}

int main(int argc, char *argv[]) {
    char file_name [256];
    int total = 0;
    int lines = 0;

    if (argc < 3) {
        fprintf(stderr, "Too few arguments");
    }

    FILE *report = fopen("pomiar_zad_2.txt", "a");

    strcpy(file_name, argv[2]);
    start_timer();
    char pattern = argv[1][0];
    int fd = open(file_name, O_RDONLY);
    char c;
    int found = 0;
    while (read(fd, &c, 1) == 1) {
        if (c == pattern) {
            found = 1;
            total++;
        }
        if (c == '\n') {
            if (found == 1) {
                lines++;
            }
            found = 0;
        }
    }

    if (found == 1) {
        lines++;
    }

    printf("total: %d; lines: %d\n", total, lines);
    close(fd);

    end_timer(report);
    fclose(report);

    return 0;
}