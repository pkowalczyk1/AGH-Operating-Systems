#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "defines.h"

mqd_t server_queue;

typedef struct Client {
    mqd_t queue;
    int id;
} Client;

Client clients[MAX_CLIENTS];

mqd_t open_server_queue() {
    struct mq_attr attr;
    attr.mq_flags = O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG;
    attr.mq_curmsgs = 0;

    errno = 0;
    mqd_t q_id = mq_open(server_queue_name, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &attr);
    if (errno == EINVAL) {
        printf("tak\n");
    }
    if (q_id == -1) {
        mq_unlink(server_queue_name);
        q_id = mq_open(server_queue_name, O_RDONLY | O_NONBLOCK | O_CREAT | O_EXCL, 0666, &attr);
        if (errno == EINVAL) {
            printf("tak\n");
        }
        if (q_id == -1) {
            fprintf(stderr, "Cannot open server queue\n");
            exit(-1);
        }
    }

    return q_id;
}

void handle_exit() {
    int client_cnt = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].queue != -1) {
            client_cnt++;
            char buffer[MAX_MSG];
            sprintf(buffer, "Server is down");
            mq_send(clients[i].queue, buffer, MAX_MSG, SERVER_DOWN);
            mq_close(clients[i].queue);
        }
    }

    int received = 0;
    while (received != client_cnt) {
        char buffer[MAX_MSG];
        unsigned int type;
        if (mq_receive(server_queue, buffer, MAX_MSG, &type) <= 0) {
            continue;
        }
        if (type == STOP) {
            received++;
        }
    }
    mq_close(server_queue);
    mq_unlink(server_queue_name);
    exit(0);
}

void handle_int(int sig) {
    handle_exit();
}

void handle_list(int sender_id) {
    int sender_queue;
    printf("Active clients:\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].queue != -1) {
            printf("%d\n", clients[i].id);
        }
        if (clients[i].id == sender_id) {
            sender_queue = clients[i].queue;
        }
    }

    char response[MAX_MSG];
    sprintf(response, "List displayed");
    mq_send(sender_queue, response, MAX_MSG, RESPONSE);
}

void handle_init(char *queue_name) {
    int i = 0;
    while (clients[i].queue != -1) {
        i++;
    }

    clients[i].queue = mq_open(queue_name, O_WRONLY | O_NONBLOCK);
    char response[MAX_MSG];
    sprintf(response, "%d", clients[i].id);
    mq_send( clients[i].queue, response, MAX_MSG, INIT);
}

void handle_stop(int sender_id) {
    int i = 0;
    while (clients[i].id != sender_id) {
        i++;
    }

    char response[MAX_MSG];
    sprintf(response, "Stopped client");
    mq_send(clients[i].queue, response, MAX_MSG, RESPONSE);
    mq_close(clients[i].queue);
    clients[i].queue = -1;
}

void handle_to_one(char *message) {
    char *token = strtok(message, " ");
    int sender_id = atoi(token);
    token = strtok(NULL, " ");
    int receiver_id = atoi(token);
    while (token[0] != '\0') {
        token += 1;
    }
    char receiver_message[MAX_MSG];
    time_t  raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);

    sprintf(receiver_message, "%d-%d-%d %d: %s", time_info->tm_mday,
            time_info->tm_mon + 1, time_info->tm_year + 1900, sender_id, token + 1);
    int receiver_queue = -1;
    int sender_queue = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].id == sender_id) {
            sender_queue = clients[i].queue;
        }
        else if (clients[i].id == receiver_id) {
            receiver_queue = clients[i].queue;
        }
    }

    char response[MAX_MSG];
    if (receiver_queue == -1) {
        sprintf(response, "Could not send message to receiver");
    }
    else {
        sprintf(response, "Message sent");
        mq_send(receiver_queue, receiver_message, MAX_MSG, TO_ONE);
    }
    mq_send(sender_queue, response, MAX_MSG, RESPONSE);
}

void handle_to_all(char *message) {
    char *token = strtok(message, " ");
    int sender_id = atoi(token);
    while (token[0] != '\0') {
        token += 1;
    }
    char receiver_message[MAX_MSG];
    time_t  raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);

    sprintf(receiver_message, "%d-%d-%d %d: %s", time_info->tm_mday,
            time_info->tm_mon + 1, time_info->tm_year + 1900, sender_id, token + 1);
    int sender_queue;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].queue != -1 && clients[i].id != sender_id) {
            mq_send(clients[i].queue, receiver_message, MAX_MSG, TO_ALL);
        }
        if (clients[i].id == sender_id) {
            sender_queue = clients[i].queue;
        }
    }

    char response[MAX_MSG];
    sprintf(response, "Messages sent");
    mq_send(sender_queue, response, MAX_MSG, RESPONSE);
}

void save_request_to_file(FILE *file, char *request, int sender_id) {
    time_t  raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);

    fprintf(file, "%d-%d-%d %d:%d %d %s\n", time_info->tm_mday,
            time_info->tm_mon + 1, time_info->tm_year + 1900, time_info->tm_hour, time_info->tm_min, sender_id, request);
}

int main() {
    server_queue = open_server_queue();
    signal(SIGINT, handle_int);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].queue = -1;
        clients[i].id = i;
    }

    FILE *log_file = fopen("log_file.txt", "w");
    fprintf(log_file, "DATE  SENDER_ID  REQUEST\n");

    while (1) {
        unsigned int type;
        char request[MAX_MSG];
        if (mq_receive(server_queue, request, MAX_MSG, &type) <= 0) {
            continue;
        }
        int sender_id;
        char file_buffer[MAX_MSG];
        char tmp_buf[MAX_MSG];
        char *token;
        sprintf(tmp_buf, "%s", request);
        switch(type) {
            case LIST:
                printf("Received LIST request\n");
                sender_id = atoi(request);
                save_request_to_file(log_file, "LIST", sender_id);
                handle_list(sender_id);
                break;
            case TO_ONE:
                printf("Received 2ONE request\n");
                token = strtok(tmp_buf, " ");
                sender_id = atoi(token);
                while (token[0] != '\0') {
                    token += 1;
                }
                sprintf(file_buffer, "2ONE %s", token + 1);
                save_request_to_file(log_file, file_buffer, sender_id);
                handle_to_one(request);
                break;
            case TO_ALL:
                printf("Received 2ALL request\n");
                token = strtok(tmp_buf, " ");
                sender_id = atoi(token);
                while (token[0] != '\0') {
                    token += 1;
                }
                sprintf(file_buffer, "2ALL %s", token + 1);
                save_request_to_file(log_file, file_buffer, sender_id);
                handle_to_all(request);
                break;
            case INIT:
                printf("Received INIT request\n");
                handle_init(request);
                break;
            case STOP:
                printf("Received STOP request\n");
                sender_id = atoi(request);
                save_request_to_file(log_file, "STOP", sender_id);
                handle_stop(sender_id);
                break;
        }
    }
}

