#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum Sending_mode {
    KILL,
    SIGQUEUE,
    SIGRT
} Sending_mode;

Sending_mode mode;
int received_cnt = 0;
int waiting = 1;
int normal_signal;
int ending_signal;

void handler(int sig, siginfo_t *info, void *ucontext) {
    if (sig == normal_signal) {
        received_cnt++;
        kill(info->si_pid, SIGUSR1);
    }
    else if (sig == ending_signal) {
        waiting = 0;
        for (int i = 0; i < received_cnt; i++) {
            if (mode == SIGQUEUE) {
                sigval_t value;
                value.sival_int = i;
                sigqueue(info->si_pid, normal_signal, value);
            }
            else {
                kill(info->si_pid, normal_signal);
            }
        }

        if (mode == SIGQUEUE) {
            sigval_t value = {received_cnt};
            sigqueue(info->si_pid, SIGUSR2, value);
        }
        else {
            kill(info->si_pid, ending_signal);
        }

        printf("Received %d signals\n", received_cnt);
    }
}

void set_sigaction(int signal, void (*handler)(int, siginfo_t*, void*)) {
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(signal, &act, NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments");
        return -1;
    }

    printf("Catcher with pid %d\n", getpid());

    if (strcmp(argv[1], "KILL") == 0) {
        mode = KILL;
        normal_signal = SIGUSR1;
        ending_signal = SIGUSR2;
    }
    else if (strcmp(argv[1], "SIGQUEUE") == 0) {
        mode = SIGQUEUE;
        normal_signal = SIGUSR1;
        ending_signal = SIGUSR2;
    }
    else if (strcmp(argv[1], "SIGRT") == 0) {
        mode = SIGRT;
        normal_signal = SIGRTMIN + 1;
        ending_signal = SIGRTMIN + 2;
    }
    else {
        fprintf(stderr, "Wrong mode");
        return -1;
    }

    sigset_t default_mask;
    sigfillset(&default_mask);
    sigprocmask(SIG_SETMASK, &default_mask, NULL);

    set_sigaction(normal_signal, handler);
    set_sigaction(ending_signal, handler);

    sigset_t  mask;
    sigemptyset(&mask);
    sigfillset(&mask);
    sigdelset(&mask, normal_signal);
    sigdelset(&mask, ending_signal);

    while (waiting) {
        sigsuspend(&mask);
    }

    return 0;
}