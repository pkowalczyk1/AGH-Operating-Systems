#include <dirent.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

struct counters {
    int normal;
    int dirs;
    int char_dev;
    int block_dev;
    int fifo;
    int links;
    int sockets;
};

void print_stat(struct stat file_stat, char *file_path, struct counters *count) {
    printf("***\n");
    printf("Path: %s\n", file_path);

    printf("Type: ");
    if (S_ISREG(file_stat.st_mode)) {
        printf("file\n");
        count->normal++;
    }
    if (S_ISDIR(file_stat.st_mode)) {
        printf("dir\n");
        count->dirs++;
    }
    if (S_ISCHR(file_stat.st_mode)) {
        printf("char dev\n");
        count->char_dev++;
    }
    if (S_ISBLK(file_stat.st_mode)) {
        printf("block dev\n");
        count->block_dev++;
    }
    if (S_ISFIFO(file_stat.st_mode)) {
        printf("fifo\n");
        count->fifo++;
    }
    if (S_ISLNK(file_stat.st_mode)) {
        printf("slink\n");
        count->links++;
    }
    if (S_ISSOCK(file_stat.st_mode)) {
        printf("sock\n");
        count->sockets++;
    }
    printf("Number of links: %ld\n", file_stat.st_nlink);
    printf("Size: %ld\n", file_stat.st_size);

    struct tm *info;
    info = localtime(&file_stat.st_atime);
    char acc_time [30];
    strftime(acc_time, 26, "%Y-%m-%d %H:%M:%S", info);
    printf("Access time: %s\n", acc_time);

    info = localtime(&file_stat.st_mtime);
    char mod_time [30];
    strftime(mod_time, 26, "%Y-%m-%d %H:%M:%S", info);
    printf("Modification time: %s\n", mod_time);

    printf("***\n");
}

void find(char *path, struct counters *count) {
    DIR *directory = opendir(path);
    struct dirent *file = readdir(directory);
    while (file != NULL) {
        char file_path [PATH_MAX];
        strcpy(file_path, path);
        strcat(file_path, "/");
        strcat(file_path, file->d_name);
        struct stat file_stat;
        lstat(file_path, &file_stat);

        if (S_ISDIR(file_stat.st_mode) && (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)) {
            file = readdir(directory);
            continue;
        }

        print_stat(file_stat, file_path, count);

        if (S_ISDIR(file_stat.st_mode)) {
            find(file_path, count);
        }

        file = readdir(directory);
    }
    closedir(directory);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Invalid number of arguments");
        return -1;
    }

    struct counters count = {
            .normal = 0,
            .dirs = 0,
            .char_dev = 0,
            .block_dev = 0,
            .fifo = 0,
            .links = 0,
            .sockets = 0
    };
    char *path = realpath(argv[1], NULL);
    find(path, &count);
    printf("TOTAL\nNormals: %d\nDirs: %d\nChar devs: %d\nBlock devs: %d\nFifo: %d\nLinks: %d\nSockets: %d\n",
           count.normal, count.dirs, count.char_dev, count.block_dev, count.fifo, count.links, count.sockets);

    free(path);
    return 0;
}