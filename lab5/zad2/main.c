#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Mode {
    DATE,
    SENDER
} Mode;

void display_mails(Mode mode) {
    FILE *mail;
    if (mode == DATE) {
        mail = popen("mail | tail -n+2 | sort -k3d", "w");
    }
    else {
        mail = popen("mail | tail -n+2", "w");
    }

    fputs("exit", mail);
    pclose(mail);
}

void send_mail(char *to, char *topic, char *content) {
    char command [1000];
    sprintf(command, "mail -s %s %s", to, topic);
    FILE *mail = popen(command, "w");
    fputs(content, mail);
    pclose(mail);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        Mode mode;
        if (strcmp(argv[1], "data") == 0) {
            mode = DATE;
        }
        else if (strcmp(argv[1], "nadawca")) {
            mode = SENDER;
        }
        else {
            fprintf(stderr, "No such mode");
            return -1;
        }
        display_mails(mode);
    }
    else if (argc == 4) {
        send_mail(argv[1], argv[2], argv[3]);
    }
    else {
        fprintf(stderr, "Give 1 or 3 arguments");
        return -1;
    }

    return 0;
}