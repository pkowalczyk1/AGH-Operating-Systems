#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct Element {
    int command_cnt;
    char *symbol;
    char *commands[30][20];
} Element;

typedef struct Command_chain {
    int commands_cnt;
    char *commands[30][20];
} Command_chain;

int get_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

void split_pipe(char *pipe, char **commands) {
    int i = 0;
    char *token = strtok(pipe, "|");
    while (token != NULL) {
        commands[i] = token;
        token = strtok(NULL, "|");
        if (commands[i][0] == ' ') {
            commands[i] = &commands[i][1];
        }
        if (commands[i][strlen(commands[i]) - 1] == ' ') {
            commands[i][strlen(commands[i]) - 1] = '\0';
        }
        i++;
    }
    commands[i] = NULL;
}

void split_command(char *command, char **arguments) {
    int i = 0;
    char *token = strtok(command, " ");
    while (token != NULL) {
        arguments[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    arguments[i] = NULL;
}

Element* parse_elements(int element_cnt, char **elements) {
    Element *parsed_elements = calloc(element_cnt + 1, sizeof(Element));
    for (int i = 0; i < element_cnt; i++) {
        char *commands [30];
        char *token = strtok(elements[i], "=");
        parsed_elements[i].symbol = token;
        parsed_elements[i].symbol[strlen(parsed_elements[i].symbol) - 1] = '\0';
        token = strtok(NULL, "=");
        token = &token[1];
        split_pipe(token, commands);

        int j = 0;
        while (commands[j] != NULL) {
            split_command(commands[j], parsed_elements[i].commands[j]);
            j++;
        }
        parsed_elements[i].command_cnt = j;
    }
    parsed_elements[element_cnt].command_cnt = -1;

    return parsed_elements;
}

Element search_for_symbol(char *symbol, Element *elements) {
    int ind = 0;
    while (elements[ind].command_cnt != -1 && strcmp(symbol, elements[ind].symbol) != 0) {
        ind++;
    }

    return elements[ind];
}

Command_chain* parse_commands(int command_cnt, char **commands, Element *elements) {
    Command_chain *chain_arr = calloc(command_cnt + 1, sizeof(Command_chain));
    for (int i = 0; i < command_cnt; i++) {
        int ind = 0;
        char *command_parts [30];
        char *token = strtok(commands[i], "|");
        while (token != NULL) {
            char *tmp_token = token;
            if (tmp_token[0] == ' ') {
                tmp_token = &tmp_token[1];
            }
            if (tmp_token[strlen(tmp_token) - 1] == ' ') {
                tmp_token[strlen(tmp_token) - 1] = '\0';
            }
            command_parts[ind] = tmp_token;
            ind++;
            token = strtok(NULL, "|");
        }

        int command_ind = 0;
        for (int j = 0; j < ind; j++) {
            Element tmp_element = search_for_symbol(command_parts[j], elements);
            for (int k = 0; k < tmp_element.command_cnt; k++) {
                int tmp_ind = 0;
                while (tmp_element.commands[k][tmp_ind] != NULL) {
                    chain_arr[i].commands[command_ind][tmp_ind] = tmp_element.commands[k][tmp_ind];
                    tmp_ind++;
                }
                chain_arr[i].commands[command_ind][tmp_ind] = NULL;
                command_ind++;
            }
        }
        chain_arr[i].commands_cnt = ind;
    }

    chain_arr[command_cnt].commands_cnt = -1;

    return chain_arr;
}

Command_chain* parse_file(char *file) {
    int ind = 0;
    int found_blank = 0;
    int is_blank = 1;
    int element_cnt = 0;
    while (!found_blank) {
        if (file[ind] == '\n' && is_blank) {
            found_blank = 1;
        }
        else if (file[ind] == '\n') {
            file[ind] = '\0';
            element_cnt++;
            is_blank = 1;
        }
        else {
            is_blank = 0;
        }
        ind++;
    }

    int command_begin_ind = ind;
    int command_cnt = 0;
    while (file[ind] != '\0') {
        if (file[ind] == '\n') {
            command_cnt++;
            file[ind] = '\0';
        }
        ind++;
    }

    char **elements = calloc(element_cnt + 1, sizeof(char*));
    char **commands = calloc(command_cnt + 1, sizeof(char*));
    ind = 0;
    int inserted = 0;
    int line_length = 0;
    while (inserted != element_cnt) {
        if (file[ind] == '\0') {
            elements[inserted] = &file[ind - line_length];
            line_length = 0;
            inserted++;
        }
        else {
            line_length++;
        }
        ind++;
    }
    elements[inserted] = NULL;

    ind = command_begin_ind;
    inserted = 0;
    line_length = 0;
    while (inserted != command_cnt) {
        if (file[ind] == '\0') {
            commands[inserted] = &file[ind - line_length];
            line_length = 0;
            inserted++;
        }
        else {
            line_length++;
        }
        ind++;
    }
    commands[inserted] = NULL;

    Element *parsed_elements = parse_elements(element_cnt, elements);
    Command_chain *parsed_commands = parse_commands(command_cnt, commands, parsed_elements);

    free(elements);
    free(commands);
    free(parsed_elements);

    return parsed_commands;
}

void exec_pipe(Command_chain command_pipe) {
    int commands_cnt = command_pipe.commands_cnt;
    int (*fds)[2] = calloc(commands_cnt - 1, sizeof(int[2]));
    for (int i = 0; i < commands_cnt - 1; i++) {
        pipe(fds[i]);
    }

    for (int i = 0; i < commands_cnt; i++) {
        if (fork() == 0) {
            if (i > 0) {
                close(fds[i - 1][1]);
                dup2(fds[i - 1][0], STDIN_FILENO);
            }
            if (i < commands_cnt - 1) {
                close(fds[i][0]);
                dup2(fds[i][1], STDOUT_FILENO);
            }
            execvp(command_pipe.commands[i][0], command_pipe.commands[i]);
            exit(0);
        }
        else {
            if (i > 0) {
                close(fds[i - 1][0]);
            }
            if (i < commands_cnt - 1) {
                close(fds[i][1]);
            }
        }
    }
    while (wait(NULL) > 0);

    free(fds);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments");
        return -1;
    }

    FILE *command_file = fopen(argv[1], "r");
    int file_size = get_file_size(command_file);
    char *file_buffer = calloc(file_size + 2, sizeof(char));
    fread(file_buffer, sizeof(char), file_size, command_file);
    fclose(command_file);
    file_buffer[file_size] = '\n';
    file_buffer[file_size + 1] = '\0';
    Command_chain* parsed_commands = parse_file(file_buffer);

    int i = 0;
    while (parsed_commands[i].commands_cnt != -1) {
        exec_pipe(parsed_commands[i]);
        i++;
    }

    free(parsed_commands);
    free(file_buffer);

    return 0;
}