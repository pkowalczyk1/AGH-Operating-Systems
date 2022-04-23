#define _XOPEN_SOURCE 500
#include <ftw.h>

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>

struct counters {
    int normal;
    int dirs;
    int char_dev;
    int block_dev;
    int fifo;
    int links;
    int sockets;
};

struct counters count = {
        .normal = 0,
        .dirs = 0,
        .char_dev = 0,
        .block_dev = 0,
        .fifo = 0,
        .links = 0,
        .sockets = 0
};

void print_stat(const struct stat *file_stat, const char *file_path) {
    printf("***\n");
    printf("Path: %s\n", file_path);

    printf("Type: ");
    if (S_ISREG(file_stat->st_mode)) {
        printf("file\n");
        count.normal++;
    }
    if (S_ISDIR(file_stat->st_mode)) {
        printf("dir\n");
        count.dirs++;
    }
    if (S_ISCHR(file_stat->st_mode)) {
        printf("char dev\n");
        count.char_dev++;
    }
    if (S_ISBLK(file_stat->st_mode)) {
        printf("block dev\n");
        count.block_dev++;
    }
    if (S_ISFIFO(file_stat->st_mode)) {
        printf("fifo\n");
        count.fifo++;
    }
    if (S_ISLNK(file_stat->st_mode)) {
        printf("slink\n");
        count.links++;
    }
    if (S_ISSOCK(file_stat->st_mode)) {
        printf("sock\n");
        count.sockets++;
    }
    printf("Number of links: %ld\n", file_stat->st_nlink);
    printf("Size: %ld\n", file_stat->st_size);

    struct tm *info;
    info = localtime(&file_stat->st_atime);
    char acc_time [30];
    strftime(acc_time, 26, "%Y-%m-%d %H:%M:%S", info);
    printf("Access time: %s\n", acc_time);

    info = localtime(&file_stat->st_mtime);
    char mod_time [30];
    strftime(mod_time, 26, "%Y-%m-%d %H:%M:%S", info);
    printf("Modification time: %s\n", mod_time);

    printf("***\n");
}

int find(const char *path, const struct stat *file_stat, int flag, struct FTW *buf) {
    print_stat(file_stat, path);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Invalid number of arguments");
        return -1;
    }

    char *path = realpath(argv[1], NULL);
    nftw(path, find, 15, FTW_PHYS);
    printf("TOTAL\nNormals: %d\nDirs: %d\nChar devs: %d\nBlock devs: %d\nFifo: %d\nLinks: %d\nSockets: %d\n",
           count.normal, count.dirs, count.char_dev, count.block_dev, count.fifo, count.links, count.sockets);

    free(path);
    return 0;
}