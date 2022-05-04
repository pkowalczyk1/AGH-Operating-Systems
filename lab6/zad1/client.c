#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "defines.h"

int client_queue;
int server_queue;
int client_id;

int is_input() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0,  &fds));
}

int open_queue() {
    key_t key = ftok(getenv("HOME"), rand() % 100);
    int queue_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    return queue_id;
}

int open_server_queue() {
    int queue_id = msgget(SERVER_KEY, 0);
    return queue_id;
}

void stop_client() {
    struct {
        long mtype;
        char mtext[MAX_MSG];
    } request;
    request.mtype = STOP;
    sprintf(request.mtext, "%d", client_id);
    msgsnd(server_queue, &request, MAX_MSG, 0);
    msgctl(client_queue, IPC_RMID, NULL);
    exit(0);
}

void handle_int(int sig) {
    stop_client();
}

void handle_stop() {
    stop_client();
}

int main() {
    srand(time(NULL));
    server_queue = open_server_queue();
    client_queue = open_queue();
    signal(SIGINT, handle_int);
    struct {
        long mtype;
        char mtext[MAX_MSG + 30];
    } request;
    request.mtype = INIT;
    sprintf(request.mtext, "%d", client_queue);
    msgsnd(server_queue, &request, MAX_MSG, IPC_NOWAIT);
    msgrcv(client_queue, &request, MAX_MSG, INIT, 0);
    client_id = atoi(request.mtext);
    size_t read;
    printf("Server responded. Got ID %d\n", client_id);
    printf(">");
    fflush(stdout);

    while (1) {
        if (is_input() > 0) {
            char *cmd_buf;
            getline(&cmd_buf, &read, stdin);
            cmd_buf = strtok(cmd_buf, "\n");
            if (strncmp(cmd_buf, "LIST", strlen("LIST")) == 0) {
                request.mtype = LIST;
                sprintf(request.mtext, "%d", client_id);
                msgsnd(server_queue, &request, MAX_MSG, IPC_NOWAIT);
                msgrcv(client_queue, &request, MAX_MSG, RESPONSE, 0);
                printf("Server responded with message: %s\n", request.mtext);
            }
            if (strncmp(cmd_buf, "2ONE", strlen("2ONE")) == 0) {
                request.mtype = TO_ONE;
                snprintf(request.mtext, MAX_MSG, "%d %s", client_id, &cmd_buf[strlen("2ONE") + 1]);
                msgsnd(server_queue, &request, MAX_MSG, IPC_NOWAIT);
                msgrcv(client_queue, &request, MAX_MSG, RESPONSE, 0);
                printf("Server responded with message: %s\n", request.mtext);
            }
            if (strncmp(cmd_buf, "2ALL", strlen("2ALL")) == 0) {
                request.mtype = TO_ALL;
                snprintf(request.mtext, MAX_MSG, "%d %s", client_id, &cmd_buf[strlen("2ALL") + 1]);
                msgsnd(server_queue, &request, MAX_MSG, IPC_NOWAIT);
                msgrcv(client_queue, &request, MAX_MSG, RESPONSE, 0);
                printf("Server responded with message: %s\n", request.mtext);
            }
            if (strncmp(cmd_buf, "STOP", strlen("STOP")) == 0) {
                handle_stop();
            }
            printf(">");
            fflush(stdout);
            free(cmd_buf);
        }
        if (msgrcv(client_queue, &request, MAX_MSG, TO_ONE, IPC_NOWAIT) > 0) {
            printf("%s\n", request.mtext);
            printf(">");
            fflush(stdout);
        }
        if (msgrcv(client_queue, &request, MAX_MSG, TO_ALL, IPC_NOWAIT) > 0) {
            printf("%s\n", request.mtext);
            printf(">");
            fflush(stdout);
        }
        if (msgrcv(client_queue, &request, MAX_MSG, SERVER_DOWN, IPC_NOWAIT) > 0) {
            stop_client();
        }
        usleep(100);
    }
}
