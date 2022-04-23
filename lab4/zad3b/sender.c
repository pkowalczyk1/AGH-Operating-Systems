#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum Sending_mode {
    KILL,
    SIGQUEUE,
    SIGRT
} Sending_mode;

int waiting = 1;
int received_cnt = 0;
int signals_to_send;
int normal_signal;
int ending_signal;
Sending_mode mode;

void handler(int sig, siginfo_t *info, void *ucontext) {
    if (sig == normal_signal) {
        received_cnt++;
        if (mode == SIGQUEUE) {
            printf("Received signal number %d\n", info->si_value.sival_int);
        }
    }
    else if (sig == ending_signal) {
        waiting = 0;
        printf("Sent %d signals, received %d signals\n", signals_to_send, received_cnt);
    }
}

void ack_handler(int sig, siginfo_t *info, void *ucontext) {
    printf("Response from catcher\n");
}

void set_sigaction(int signal, void (*handler)(int, siginfo_t*, void*)) {
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(signal, &act, NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Too few arguments");
        return -1;
    }

    printf("Sender with pid %d\n", getpid());

    int catcher_pid = atoi(argv[1]);
    signals_to_send = atoi(argv[2]);

    if (strcmp(argv[3], "KILL") == 0) {
        mode = KILL;
        normal_signal = SIGUSR1;
        ending_signal = SIGUSR2;
    }
    else if (strcmp(argv[3], "SIGQUEUE") == 0) {
        mode = SIGQUEUE;
        normal_signal = SIGUSR1;
        ending_signal = SIGUSR2;
    }
    else if (strcmp(argv[3], "SIGRT") == 0) {
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

    set_sigaction(SIGUSR1, ack_handler);

    for (int i = 0; i < signals_to_send; i++) {
        if (mode == SIGQUEUE) {
            sigval_t value;
            value.sival_int = 0;
            sigqueue(catcher_pid, normal_signal, value);
        }
        else {
            kill(catcher_pid, normal_signal);
        }

        sigset_t wait_mask;
        sigfillset(&wait_mask);
        sigdelset(&wait_mask, SIGUSR1);
        sigsuspend(&wait_mask);
    }

    if (mode == SIGQUEUE) {
        sigval_t value = {0};
        sigqueue(catcher_pid, ending_signal, value);
    }
    else {
        kill(catcher_pid, ending_signal);
    }

    set_sigaction(normal_signal, handler);
    set_sigaction(ending_signal, handler);

    sigset_t mask;
    sigemptyset(&mask);
    sigfillset(&mask);
    sigdelset(&mask, normal_signal);
    sigdelset(&mask, ending_signal);

    while (waiting) {
        sigsuspend(&mask);
    }

    return 0;
}