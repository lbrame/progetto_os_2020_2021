/// @file client.c
/// @brief Contiene l'implementazione del client.

#include "err_exit.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "defines.h"
#include <stdio.h>

typedef struct {
    int id;
    int delay;
    char target[3];
    char action[14];
} hackler_struct;

typedef struct {
    char senderID[2];
    pid_t pid;
} pid_struct;


/**
 * parse the read text to create a list of structures
 * @param inputBuffer the buffer of read text
 * @param fileSize the size of the buffer
 * @return a pointer to a list of hackler_struct
 */
hackler_struct *parse_hackler_struct(char *inputBuffer, int message_number) {
    hackler_struct *messages = malloc(message_number * (int) sizeof(hackler_struct));
    char *row_context;
    char *field_context;
    int row_counter = 0;

    // iterate over the rows
    for (char *row_token = strtok_r(inputBuffer, "\n", &row_context); row_token; row_token = strtok_r(NULL, "\n",
                                                                                                      &row_context)) {
        // row_counter at 0 is the first row which contains the field names
        if (row_counter != 0) {
            int field_counter = 0;
            hackler_struct *message = malloc(sizeof(hackler_struct));
            // iterate over the fields
            for (char *field_token = strtok_r(row_token, ";", &field_context); field_token; field_token = strtok_r(NULL,
                                                                                                                   ";",
                                                                                                                   &field_context)) {
                switch (field_counter) {
                    case 0:
                        message->id = atoi(field_token);
                        break;
                    case 1:
                        message->delay = atoi(field_token);
                        break;
                    case 2:
                        if(strcmp(field_token, "ShutDown") == 0) {
                            strcpy(message->action, field_token);
                            break;
                        } else {
                            strcpy(message->target, field_token);
                            break;
                        }
                    case 3:
                        strcpy(message->action, field_token);
                        break;
                    default:
                        ErrExit("parse_file");
                }
                field_counter++;
            }
            messages[row_counter - 1] = *message;
        }
        row_counter++;
    }
    return messages;
}


pid_struct *parse_pid_struct(char *inputBuffer, int message_number) {
    pid_struct *messages = malloc(message_number * (int) sizeof(pid_struct));
    char *row_context;
    char *field_context;
    int row_counter = 0;

    // iterate over the rows
    for (char *row_token = strtok_r(inputBuffer, "\n", &row_context); row_token; row_token = strtok_r(NULL, "\n",
                                                                                                      &row_context)) {
        // row_counter at 0 is the first row which contains the field names
        if (row_counter != 0) {
            int field_counter = 0;
            pid_struct *message = malloc(sizeof(pid_struct));
            // iterate over the fields
            for (char *field_token = strtok_r(row_token, ";", &field_context); field_token; field_token = strtok_r(NULL,
                                                                                                                   ";",
                                                                                                                   &field_context)) {
                switch (field_counter) {
                    case 0:
                        memcpy(message->senderID, field_token, sizeof(&field_token));
                        break;
                    case 1:
                        message->pid = atoi(field_token);
                        break;
                    default:
                        ErrExit("parse_file");
                }
                field_counter++;
            }
            messages[row_counter - 1] = *message;
        }
        row_counter++;
    }
    return messages;
}

/**
 * Function to handle outgoing signals from Hackler to other processes.
 * @param sender_messages
 * @param receiver_messages
 * @param messages
 * SIGTERM: terminate the process gracefully, properly closing all open IPCs
 * SIGUSR1: catch IncreaseDelay message (sent by hackler)
 * SIGUSR2 RemoveMsg message (sent by hackler)
 * SIGQUIT: catch SendMsg message (sent by hackler)
 */
void handle_signals(pid_struct *sender_messages, pid_struct *receiver_messages, hackler_struct *messages,
                    int message_number) {
    printf("called handle_signals\n");
    printf("message number: %d\n", message_number);
    for (int i = 0; i < message_number; i++) {
        printf("Number %d for loop\n", i);
        printf("message: %s\n", messages[i].action);
        hackler_struct message = messages[i];
        sleep(message.delay);
        pid_struct reciver_info;

        if(strcmp("S1", message.target) == 0) {
            reciver_info = sender_messages[0];
        } else if(strcmp("S2", message.target) == 0) {
            reciver_info = sender_messages[1];
        }else if(strcmp("S3", message.target) == 0) {
            reciver_info = sender_messages[2];
        } else if(strcmp("R1", message.target) == 0) {
            reciver_info = receiver_messages[0];
        } else if(strcmp("R2", message.target) == 0) {
            reciver_info = receiver_messages[1];
        }else if(strcmp("R3", message.target) == 0) {
            reciver_info = receiver_messages[2];
        }


        if (strcmp(message.action, "IncreaseDelay") == 0) {
            printf("IncreaseDelay\n");
            kill(reciver_info.pid, SIGUSR1);

        } else if (strcmp(message.action, "RemoveMSG") == 0) {
            printf("RemoveMsg\n");
            kill(reciver_info.pid, SIGUSR2);

        } else if (strcmp(message.action, "SendMSG") == 0) {
            printf("SendMsg\n");
            kill(reciver_info.pid, SIGQUIT);

        } else if (strcmp(message.action, "ShutDown") == 0) {
            printf("Shutdown\n");
            for (int j = 0; j < 3; j++) {
                pid_struct sender_message = sender_messages[j];
                pid_struct receiver_message = receiver_messages[2 - j];
                kill(sender_message.pid, SIGTERM);
                kill(receiver_message.pid, SIGTERM);
                printf("Hackler sent SIGTERM to:\n"
                       "Sender messages:"
                       "\t%s %d\n"
                       "Receiver messages:"
                       "\t%s %d \n",
                       sender_message.senderID, sender_message.pid,
                       receiver_message.senderID, receiver_message.pid);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    printf("Hackler started\n");

    // READ F7.csv
    // argv[1] is the relative path to the input file and it is passes as a keyboard argument
    int fileSize;
    do {
        fileSize = get_file_size(argv[1]);
    } while (fileSize == -1 || fileSize == 0);
    printf("F7 Filesize = %d\n", fileSize);

    // allocate buffer to read file of size fileSize + 1(space for /0)
    char *inputBuffer = malloc(sizeof(char) * fileSize + 1);
    if (inputBuffer == NULL) {
        ErrExit("malloc");
    }
    read_file(inputBuffer, argv[1], fileSize);
    int message_number = count_messages(inputBuffer, fileSize);
    hackler_struct *messages = parse_hackler_struct(inputBuffer, message_number);
    hackler_struct message_shtdwn = messages[3];
    free(inputBuffer);

    // READ f8.csv
    char path_sender[] = "OutputFiles/F8.csv";
    do {
        fileSize = get_file_size(path_sender);
    } while (fileSize == -1 || fileSize == 0);
    printf("F8 Filesize = %d\n", fileSize);


    // allocate buffer to read file of size fileSize + 1(space for /0)
    inputBuffer = malloc(sizeof(char) * fileSize + 1);
    if (inputBuffer == NULL)
        ErrExit("malloc");


    read_file(inputBuffer, path_sender, fileSize);
    int n = count_messages(inputBuffer, fileSize);
    pid_struct *sender_messages = parse_pid_struct(inputBuffer, n);
    free(inputBuffer);
    printf("READ F8\n");

    // READ f9.csv
    char path_receiver[] = "OutputFiles/F9.csv";
    do {
        fileSize = get_file_size(path_receiver);
    } while (fileSize == -1 || fileSize == 0);
    printf("F9 Filesize = %d\n", fileSize);
    // allocate buffer to read file of size fileSize + 1(space for /0)
    inputBuffer = malloc(sizeof(char) * fileSize + 1);
    if (inputBuffer == NULL)
        ErrExit("malloc");

    read_file(inputBuffer, path_receiver, fileSize);
    n = count_messages(inputBuffer, fileSize);
    pid_struct *receiver_messages = parse_pid_struct(inputBuffer, n);
    free(inputBuffer);
    printf("READ F9\n");

    handle_signals(sender_messages, receiver_messages, messages, message_number);


    free(messages);
    free(sender_messages);
    free(receiver_messages);

    printf("Hackler quitting: %d\n", getpid());

    return 0;
}
