#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/times.h>

#define EPS 0.000000001

clock_t start_time, end_time;
struct tms start_cpu;
struct tms end_cpu;

void start_timer() {
    start_time = times(&start_cpu);
}

void end_timer(FILE *file) {
    int clock = sysconf(_SC_CLK_TCK);
    end_time = times(&end_cpu);
    fprintf(file, "Real: %f, User: %f, System: %f\n",
            (double) (end_time - start_time) / clock,
            (double) (end_cpu.tms_utime - start_cpu.tms_utime) / clock,
            (double) (end_cpu.tms_stime - start_cpu.tms_stime) / clock);

    printf("Real: %f, User: %f, System: %f\n",
           (double) (end_time - start_time) / clock,
           (double) (end_cpu.tms_utime - start_cpu.tms_utime) / clock,
           (double) (end_cpu.tms_stime - start_cpu.tms_stime) / clock);
}

double func(double x) {
    return 4 / (x * x + 1);
}

void count_integral_part(double start, double end, double step, int proc) {
    if (end > 1) {
        end = 1;
    }

    double result = 0;
    double prev;
    while (start - end < -1 * EPS) {
        prev = start;
        start += step;
        if (start > 1) {
            start = end;
        }
        result += func(start) * (start - prev);
    }

    char filename [256] = "W";
    char proc_num [20];
    sprintf(proc_num, "%d.txt", proc);
    strcat(filename, proc_num);
    FILE *file = fopen(filename, "w");
    fprintf(file, "%lf", result);
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Invalid number of arguments");
        return -1;
    }

    FILE *report = fopen("report2.txt", "a");
    start_timer();

    double length = atof(argv[1]);
    int int_cnt = (int) (1 / length);
    if (int_cnt * length < 1) {
        int_cnt++;
    }

    int proc_cnt = atoi(argv[2]);
    int ints_for_proc = (int) (int_cnt / proc_cnt);

    double start = 0;
    for (int i = 1; i <= proc_cnt; i++) {
        if (fork() == 0) {
            if (i != proc_cnt) {
                count_integral_part(start, start + ints_for_proc * length, length, i);
            }
            else {
                count_integral_part(start, 1, length, i);
            }
            exit(0);
        }
        start += ints_for_proc * length;
    }

    for (int i = 0; i < proc_cnt; i++) {
        wait(NULL);
    }

    double result = 0;
    for (int i = 1; i <= proc_cnt; i++) {
        char filename [256] = "W";
        char proc_num [20] = "";
        sprintf(proc_num, "%d.txt", i);
        strcat(filename, proc_num);
        FILE *file = fopen(filename, "r");
        char result_buf [256];
        fgets(result_buf, 256, file);
        double result_part = atof(result_buf);
        result += result_part;
        fclose(file);
        remove(filename);
    }

    printf("%lf\n", result);

    fprintf(report, "\nInterval: %E, Process count: %d\n", length, proc_cnt);
    end_timer(report);
    fclose(report);

    return 0;
}