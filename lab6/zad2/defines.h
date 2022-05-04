#ifndef LAB6_DEFINES_H
#define LAB6_DEFINES_H

#define MAX_CLIENTS 20
#define MAX_MSG 1024

const char server_queue_name[] = "/server_queue";

typedef enum Msg_type {
    INIT = 1,
    SERVER_DOWN,
    RESPONSE,
    TO_ALL,
    TO_ONE,
    LIST,
    STOP
} Msg_type;

#endif //LAB6_DEFINES_H
