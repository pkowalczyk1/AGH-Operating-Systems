#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
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
            "System functions",
            (double) (end_time - start_time) / clock,
            (double) (end_cpu.tms_utime - start_cpu.tms_utime) / clock,
            (double) (end_cpu.tms_stime - start_cpu.tms_stime) / clock);
}

int main(int argc, char *argv[]) {
    char source_file[256];
    char dest_file[256];
    int sfd;
    int dfd;

    FILE *report = fopen("pomiar_zad_1.txt", "a");

    if (argc < 3) {
        printf("Ścieżka do pliku kopiowanego: ");
        scanf("%s", source_file);
        printf("Ścieżka do pliku docelowego: ");
        scanf("%s", dest_file);
    }
    else {
        strcpy(source_file, argv[1]);
        strcpy(dest_file, argv[2]);
    }

    sfd = open(source_file, O_RDONLY);
    dfd = open(dest_file, O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

    start_timer();

    char c;
    int size = 0;
    int is_blank = 1;
    while (read(sfd, &c, 1) == 1) {
        size++;
        if (c != ' ' && c != '\t' && c != '\r' && c != '\0' && c != '\n') {
            is_blank = 0;
        }
        if ((c == '\n' || c == '\0' || c == '\r') && is_blank != 1) {
            lseek(sfd, -1 * size, SEEK_CUR);
            char *buf = calloc(size, sizeof(char));
            read(sfd, buf, size);
            write(dfd, buf, size);
            free(buf);
            is_blank = 1;
            size = 0;
        }
        else if (c == '\n' || c == '\0' || c == '\r') {
            size = 0;
            is_blank = 1;
        }
    }

    if (is_blank != 1) {
        lseek(sfd, -1 * size, SEEK_CUR);
        char *buf = calloc(size, sizeof(char));
        read(sfd, buf, size);
        write(dfd, buf, size);
        free(buf);
    }

    close(sfd);
    close(dfd);

    end_timer(report);
    fclose(report);

    return 0;
}