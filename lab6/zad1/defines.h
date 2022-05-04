#ifndef LAB6_DEFINES_H
#define LAB6_DEFINES_H

#define MAX_CLIENTS 20
#define MAX_MSG 1024
#define SERVER_KEY 555224

typedef enum Msg_type {
    STOP = 1,
    LIST,
    TO_ONE,
    TO_ALL,
    RESPONSE,
    SERVER_DOWN,
    INIT
} Msg_type;

#endif //LAB6_DEFINES_H
