#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "defines.h"

int running = 1;
int server_queue;

typedef struct Client {
    int queue;
    int id;
} Client;

Client clients[MAX_CLIENTS];

int open_server_queue() {
    errno = 0;
    key_t key = SERVER_KEY;
    msgctl(key, IPC_RMID, NULL);
    int q_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (q_id == -1) {
        int flag = 0;
        if (errno == EEXIST) {
            q_id = msgget(key, 0);
            if (msgctl(q_id, IPC_RMID, 0) == -1) {
                flag = 1;
            }
            else {
                q_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
            }
            if (flag) {
                fprintf(stderr, "Cannot create server queue\n");
                exit(-1);
            }
        }
    }

    return q_id;
}

void handle_exit() {
    int client_cnt = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].queue != -1) {
            client_cnt++;
            struct {
                long mtype;
                char mtext[MAX_MSG];
            } response;
            response.mtype = SERVER_DOWN;
            sprintf(response.mtext, "Server is down");
            msgsnd(clients[i].queue, &response, MAX_MSG, IPC_NOWAIT);
        }
    }

    int received = 0;
    while (received != client_cnt) {
        struct {
            long mtype;
            char mtext[MAX_MSG];
        } request;
        int read = msgrcv(server_queue, &request, MAX_MSG, -100, MSG_NOERROR);
        request.mtext[read] = '\0';
        received++;
    }
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

    struct {
        long mtype;
        char mtext[MAX_MSG];
    } response;
    response.mtype = RESPONSE;
    sprintf(response.mtext, "List displayed");
    msgsnd(sender_queue, &response, MAX_MSG, IPC_NOWAIT);
}

void handle_init(int sender_queue) {
    int i = 0;
    while (clients[i].queue != -1) {
        i++;
    }

    clients[i].queue = sender_queue;
    struct {
        long mtype;
        char mtext[MAX_MSG];
    } init_response;
    init_response.mtype = INIT;
    sprintf(init_response.mtext, "%d", clients[i].id);
    msgsnd(sender_queue, &init_response, MAX_MSG, IPC_NOWAIT);
}

void handle_stop(int sender_id) {
    int i = 0;
    while (clients[i].id != sender_id) {
        i++;
    }

    struct {
        long mtype;
        char mtext[MAX_MSG];
    } response;
    response.mtype = RESPONSE;
    sprintf(response.mtext, "Stopped client");
    msgsnd(clients[i].queue, &response, MAX_MSG, IPC_NOWAIT);
    clients[i].queue = -1;
}

void handle_to_one(char *message) {
    char *token = strtok(message, " ");
    int sender_id = atoi(token);
    token = strtok(NULL, " ");
    int receiver_id = atoi(token);
    struct {
        long mtype;
        char mtext[MAX_MSG];
    } receiver_message;
    receiver_message.mtype = TO_ONE;
    time_t  raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);

    sprintf(receiver_message.mtext, "%d-%d-%d %d: %s", time_info->tm_mday,
            time_info->tm_mon + 1, time_info->tm_year + 1900, sender_id, token + 2);
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

    struct {
        long mtype;
        char mtext[MAX_MSG];
    } response;
    response.mtype = RESPONSE;
    if (receiver_queue == -1) {
        sprintf(response.mtext, "Could not send message to receiver");
    }
    else {
        sprintf(response.mtext, "Message sent");
        msgsnd(receiver_queue, &receiver_message, MAX_MSG, IPC_NOWAIT);
    }
    msgsnd(sender_queue, &response, MAX_MSG, IPC_NOWAIT);
}

void handle_to_all(char *message) {
    char *token = strtok(message, " ");
    int sender_id = atoi(token);
    while (token[0] != '\0') {
        token += 1;
    }
    struct {
        long mtype;
        char mtext[MAX_MSG];
    } receiver_message;
    receiver_message.mtype = TO_ALL;
    time_t  raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);

    sprintf(receiver_message.mtext, "%d-%d-%d %d: %s", time_info->tm_mday,
            time_info->tm_mon + 1, time_info->tm_year + 1900, sender_id, token + 1);
    int sender_queue;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].queue != -1 && clients[i].id != sender_id) {
            msgsnd(clients[i].queue, &receiver_message, MAX_MSG, IPC_NOWAIT);
        }
        if (clients[i].id == sender_id) {
            sender_queue = clients[i].queue;
        }
    }

    struct {
        long mtype;
        char mtext[MAX_MSG];
    } response;
    response.mtype = RESPONSE;
    sprintf(response.mtext, "Messages sent");
    msgsnd(sender_queue, &response, MAX_MSG, IPC_NOWAIT);
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
        struct {
            long mtype;
            char mtext[MAX_MSG];
        } request;
        int read = msgrcv(server_queue, &request, MAX_MSG, -100, MSG_NOERROR);
        request.mtext[read] = '\0';
        int sender_id;
        char file_buffer[MAX_MSG];
        char tmp_buf[MAX_MSG];
        char *token;
        sprintf(tmp_buf, "%s", request.mtext);
        switch(request.mtype) {
            case LIST:
                printf("Received LIST request\n");
                sender_id = atoi(request.mtext);
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
                handle_to_one(request.mtext);
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
                handle_to_all(request.mtext);
                break;
            case INIT:
                printf("Received INIT request\n");
                int sender_queue = atoi(request.mtext);
                handle_init(sender_queue);
                break;
            case STOP:
                printf("Received STOP request\n");
                sender_id = atoi(request.mtext);
                save_request_to_file(log_file, "STOP", sender_id);
                handle_stop(sender_id);
                break;
        }
    }
}
