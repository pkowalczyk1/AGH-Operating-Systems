#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <errno.h>
#include "defines.h"

mqd_t client_queue;
mqd_t server_queue;
char queue_name[1024];
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

mqd_t open_queue() {
    pid_t pid = getpid();
    sprintf(queue_name, "/client_queue_%d;", pid);
    struct mq_attr attr;
    attr.mq_flags = O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG;
    attr.mq_curmsgs = 0;

    errno = 0;
    mqd_t q_id = mq_open(queue_name, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &attr);
    if (q_id == -1) {
        mq_unlink(queue_name);
        q_id = mq_open(queue_name, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &attr);
        if (q_id == -1) {
            fprintf(stderr, "Cannot open client queue\n");
            exit(-1);
        }
    }

    return q_id;
}

mqd_t open_server_queue() {
    mqd_t q_id = mq_open(server_queue_name, O_WRONLY);
    return q_id;
}

void stop_client() {
    char request[MAX_MSG];
    sprintf(request, "%d", client_id);
    mq_send(server_queue, request, MAX_MSG, STOP);
    unsigned int type;
    while (mq_receive(client_queue, request, MAX_MSG, &type) <= 0);
    mq_close(client_queue);
    mq_close(server_queue);
    mq_unlink(queue_name);
    exit(0);
}

void handle_server_down() {
    char request[MAX_MSG];
    sprintf(request, "%d", client_id);
    mq_send(server_queue, request, MAX_MSG, STOP);
    mq_close(client_queue);
    mq_close(server_queue);
    mq_unlink(queue_name);
    exit(0);
}

void handle_int(int sig) {
    stop_client();
}

void handle_stop() {
    stop_client();
}

int main() {
    server_queue = open_server_queue();
    client_queue = open_queue();
    signal(SIGINT, handle_int);
    char request[MAX_MSG];
    sprintf(request, "%s", queue_name);
    mq_send(server_queue, request, MAX_MSG, INIT);
    while (mq_receive(client_queue, request, MAX_MSG, NULL) <= 0);
    client_id = atoi(request);
    size_t read;
    printf("Server responded. Got ID %d\n", client_id);
    printf(">");
    fflush(stdout);

    while (1) {
        if (is_input() > 0) {
            char *cmd_buf = NULL;
            getline(&cmd_buf, &read, stdin);
            cmd_buf = strtok(cmd_buf, "\n");
            if (strncmp(cmd_buf, "LIST", strlen("LIST")) == 0) {
                sprintf(request, "%d", client_id);
                mq_send(server_queue, request, MAX_MSG, LIST);
                while (mq_receive(client_queue, request, MAX_MSG, NULL) <= 0);
                printf("Server responded with message: %s\n", request);
            }
            if (strncmp(cmd_buf, "2ONE", strlen("2ONE")) == 0) {
                snprintf(request, MAX_MSG, "%d %s", client_id, &cmd_buf[strlen("2ONE") + 1]);
                mq_send(server_queue, request, MAX_MSG, TO_ONE);
                while (mq_receive(client_queue, request, MAX_MSG, NULL) <= 0);
                printf("Server responded with message: %s\n", request);
            }
            if (strncmp(cmd_buf, "2ALL", strlen("2ALL")) == 0) {
                snprintf(request, MAX_MSG, "%d %s", client_id, &cmd_buf[strlen("2ALL") + 1]);
                mq_send(server_queue, request, MAX_MSG, TO_ALL);
                while (mq_receive(client_queue, request, MAX_MSG, NULL) <= 0);
                printf("Server responded with message: %s\n", request);
            }
            if (strncmp(cmd_buf, "STOP", strlen("STOP")) == 0) {
                handle_stop();
            }
            printf(">");
            fflush(stdout);
            free(cmd_buf);
        }
        unsigned int type;
        if (mq_receive(client_queue, request, MAX_MSG, &type) > 0) {
            if (type == TO_ALL || type == TO_ONE) {
                printf("%s\n", request);
                printf(">");
                fflush(stdout);
            }
            if (type == SERVER_DOWN) {
                handle_server_down();
            }
        }
        usleep(100);
    }
}

