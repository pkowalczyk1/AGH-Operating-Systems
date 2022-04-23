#include "library1.h"
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

void end_timer(FILE *file, char *name) {
    int clock = sysconf(_SC_CLK_TCK);
    end_time = times(&end_cpu);
    fprintf(file, "%s:\nReal: %f, User: %f, System: %f\n",
            name,
            (double) (end_time - start_time) / clock,
            (double) (end_cpu.tms_utime - start_cpu.tms_utime) / clock,
            (double) (end_cpu.tms_stime - start_cpu.tms_stime) / clock);
}

int main(int argc, char *argv[]) {
    FILE *report = fopen("raport2.txt", "w");
    int i = 1;
    while (i < argc) {
        char *command = argv[i++];
        if (strcmp(command, "create_table") == 0) {
            if (i + 1 > argc) {
                fprintf(stderr, "Invalid number of arguments for create_table");
                return -1;
            }
            int array_size = atoi(argv[i++]);
            if (array_size < 0) {
                fprintf(stderr, "Array size cannot be negative");
                return -1;
            }
            create_table(array_size);
        }
        else if (strcmp(command, "wc_files") == 0) {
            if (i + 1 > argc) {
                fprintf(stderr, "Invalid number of arguments for wc_files");
                return -1;
            }
            if (wc_files(argv[i++]) != 0) {
                fprintf(stderr, "Problem with running wc on given files");
                return -1;
            }
        }
        else if (strcmp(command, "save_tmp") == 0) {
            int index = save_tmp_to_array();
            if (index < 0) {
                fprintf(stderr, "Problem with saving temporary file to array");
                return -1;
            }
        }
        else if (strcmp(command, "remove_block") == 0) {
            if (i + 1 > argc) {
                fprintf(stderr, "Invalid number of arguments for remove_block");
                return -1;
            }
            int index = atoi(argv[i++]);
            if (index < 0) {
                fprintf(stderr, "Index to remove cannot be negative");
                return -1;
            }
        }
        else if (strcmp(command, "measure_time") == 0) {
            start_timer();
        }
        else if (strcmp(command, "end_measure") == 0) {
            if (i + 1 > argc) {
                fprintf(stderr, "Invalid number of arguments for end_measure");
                return -1;
            }
            end_timer(report, argv[i++]);
        }
    }

    free_array();
    fclose(report);

    return 0;
}
