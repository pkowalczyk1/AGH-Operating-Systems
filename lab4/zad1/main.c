#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

void handler(int sig_no) {
    printf("Received signal %d in process %d\n", sig_no, getpid());
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments");
        return -1;
    }

    if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, handler);
    }
    else if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &mask, NULL);
    }
    else {
        fprintf(stderr, "Unknown command");
        return -1;
    }

    raise(SIGUSR1);
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        sigset_t set;
        sigpending(&set);
        if (sigismember(&set, SIGUSR1)) {
            printf("Signal %d is pending in parent process\n", SIGUSR1);
        }
    }

    if (fork() == 0) {
        if (strcmp(argv[1], "pending") != 0) {
            raise(SIGUSR1);
        }
        if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
            sigset_t set;
            sigpending(&set);
            if (sigismember(&set, SIGUSR1)) {
                printf("Signal %d is pending in child process\n", SIGUSR1);
            }
        }
        exit(0);
    }

    execl("./test", "test", NULL);

    return 0;
}