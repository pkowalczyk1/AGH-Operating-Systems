#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int nodefer_cnt = 0;

void sigusr1_handler(int signal, siginfo_t *info, void *ucontext) {
    printf("Signal number: %d\n", signal);
    printf("PID of process: %d\n", info->si_pid);
    printf("UID of sender: %d\n", info->si_uid);
    printf("System time consumed: %ld\n", info->si_stime);
    printf("Exit value or signal: %d\n", info->si_status);
}

void sigint_handler(int signal, siginfo_t *info, void *ucontext) {
    printf("Received SIGINT\n");
}

void sigusr2_handler(int signal, siginfo_t *info, void *ucontext) {
    nodefer_cnt++;
    printf("Received SIGUSR2; current count: %d; testing SA_NODEFER flag\n", nodefer_cnt);
    if (nodefer_cnt < 5) {
        raise(SIGUSR2);
    }
}

void set_sigaction(int signal, int flag, void (*handler)(int, siginfo_t*, void*)) {
    struct sigaction act;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = flag;
    sigaction(signal, &act, NULL);
}

int main() {
    printf("***Testing SA_SIGINFO flag***\n");
    set_sigaction(SIGUSR1, SA_SIGINFO, sigusr1_handler);
    raise(SIGUSR1);

    printf("***Testing SA_NODEFER flag***\n");
    set_sigaction(SIGUSR2, SA_NODEFER, sigusr2_handler);
    raise(SIGUSR2);

    printf("***Testing SA_RESETHAND flag***\n");
    set_sigaction(SIGINT, SA_RESETHAND, sigint_handler);
    printf("SIGINT calls handler\n");
    raise(SIGINT);
    printf("Now SIGINT should terminate process\n");
    raise(SIGINT);

    return 0;
}