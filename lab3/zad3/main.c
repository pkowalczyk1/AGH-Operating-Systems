#include <stdio.h>
#include <dirent.h>
#include <wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int get_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}

int check_for_pattern(char *file_path, char *pattern) {
    FILE *file = fopen(file_path, "r");
    int size = get_file_size(file);
    char *buffer = calloc(size, sizeof(char));
    fread(buffer, sizeof(char), size, file);
    fclose(file);
    if (strstr(buffer, pattern) == NULL) {
        free(buffer);
        return 0;
    }
    free(buffer);

    return 1;
}

void search(char *path, char *pattern, int max_depth, char *relative) {
    printf("***START***\nPATH: %s; PID: %d\n", relative, getpid());
    DIR *directory = opendir(path);
    struct dirent *file = readdir(directory);

    while (file != NULL) {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            file = readdir(directory);
            continue;
        }

        char file_path[300];
        sprintf(file_path, "%s/%s", path, file->d_name);
        if (file->d_type == DT_DIR) {
            if (fork() == 0) {
                char new_relative [300];
                sprintf(new_relative, "%s%s/", relative, file->d_name);
                char max_depth_str [20];
                sprintf(max_depth_str, "%d", max_depth - 1);

                execl("./main", "main", file_path, pattern, max_depth_str, new_relative, NULL);
            }
            else {
                wait(NULL);
            }
        }
        else if (strlen(file->d_name) > 4 && strcmp(&file->d_name[strlen(file->d_name) - 4], ".txt") == 0) {
            if (check_for_pattern(file_path, pattern) == 1) {
                printf("%s\n", file->d_name);
            }
        }
        file = readdir(directory);
    }
    printf("***END***\n");
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Invalid number of arguments");
        return -1;
    }

    char *path = argv[1];
    char *pattern = argv[2];
    int max_depth = atoi(argv[3]);

    if (max_depth < 0) {
        return 0;
    }

    char *relative_path = "/";
    if (argc > 4) {
        relative_path = argv[4];
    }

    search(path, pattern, max_depth, relative_path);

    return 0;
}