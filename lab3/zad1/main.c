#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments");
        return -1;
    }

    int number = atoi(argv[1]);
    for (int i = 0; i < number; i++) {
        if (fork() == 0) {
            pid_t pid = getpid();
            printf("Message from process with PID %d\n", pid);
            break;
        }
    }

    return 0;
}