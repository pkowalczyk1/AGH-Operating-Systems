#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

int is_pending() {
    sigset_t set;
    sigpending(&set);
    return sigismember(&set, SIGUSR1);
}

int main() {
    if (is_pending()) {
        printf("Signal %d is pending after exec before raise\n", SIGUSR1);
    }
    raise(SIGUSR1);
    if (is_pending()) {
        printf("Signal %d is pending after exec after raise\n", SIGUSR1);
    }

    return 0;
}