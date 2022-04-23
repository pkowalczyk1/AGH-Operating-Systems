#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>

int main() {
    mkfifo("test_fifo", 0666);

    if (fork() == 0) {
        execl("./producer", "producer", "test_fifo", "10", "test_files/test1.txt", "100", NULL);
        exit(0);
    }
    if (fork() == 0) {
        execl("./producer", "producer", "test_fifo", "5", "test_files/test2.txt", "100", NULL);
        exit(0);
    }
    if (fork() == 0) {
        execl("./producer", "producer", "test_fifo", "2", "test_files/test3.txt", "100", NULL);
        exit(0);
    }
    if (fork() == 0) {
        execl("./producer", "producer", "test_fifo", "15", "test_files/test4.txt", "100", NULL);
        exit(0);
    }
    if (fork() == 0) {
        execl("./producer", "producer", "test_fifo", "12", "test_files/test5.txt", "100", NULL);
        exit(0);
    }

    if (fork() == 0) {
        execl("./consumer", "consumer", "test_fifo", "output_test.txt", "100", NULL);
        exit(0);
    }

    while (wait(NULL) > 0);

    return 0;
}