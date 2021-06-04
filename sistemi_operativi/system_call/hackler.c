/// @file client.c
/// @brief Contiene l'implementazione del client.

#include "err_exit.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "defines.h"

typedef struct {
    int id;
    int delay;
    char target[2];
    char action[13];
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
                        memcpy(message->target, field_token, sizeof(&field_token));
                        break;
                    case 3:
                        memcpy(message->action, field_token, sizeof(&field_token));
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
 * join all messages preparing the text to be outputted to file
 * @param messages a list containing all messages read from the file which are encapsulated into a structure
 * @param starter the starting string containing field names
 * @param message_number the number of messages contained into the input file
 * @return the string to be outputted to file
 */
/*char *concatenation(hackler_struct *messages, char *starter, int message_number) {
    // buffer used to save messages in order to return them to the main func
    char* outputBuffer;
    // buffer used too free memory
    char* old_outputBuffer;
    // first for cycle to iterate messagges array
    for(int row = message_number - 1; row >= 0; row--){
        //second for cycle to iterate all the arguments of the structure and save them in the right buffer
        for(int field_n = 0; field_n <= 6; field_n++) {
            switch (field_n) {
                case 0:
                    //if it's the first row
                    if(row == message_number-1) {
                        outputBuffer = join(messages[row].id, "", NULL);
                    } else {
                        old_outputBuffer = outputBuffer;
                        //join function = needed to save all the arguments in the buffers, divided by a third variable(could be null or another char)
                        outputBuffer = join(outputBuffer, messages[row].id, NULL);
                        free(old_outputBuffer);
                    }
                    break;
                case 1:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join( outputBuffer, messages[row].message, ';');
                    free(old_outputBuffer);
                    break;
                case 2:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, messages[row].id_sender, ';');
                    free(old_outputBuffer);
                    break;
                case 3:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join( outputBuffer,messages[row].id_receiver, ';');
                    free(old_outputBuffer);
                    break;
                case 4:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join( outputBuffer,messages[row].time_arrival, ';');
                    free(old_outputBuffer);
                    break;
                case 5:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join( outputBuffer,messages[row].time_departure, ';');
                    free(old_outputBuffer);
                    break;
                case 6:
                    old_outputBuffer = outputBuffer;
                    //if it's not the first row
                    if (row > 0)
                        outputBuffer = join( outputBuffer, "\n", NULL);
                    else
                        outputBuffer = join( outputBuffer,"\0", ';');
                    free(old_outputBuffer);
                    break;
                default:
                    ErrExit("concatenation");
                }
            }
        }
        //append the contents of buffer to the line "starter"
        outputBuffer = join(starter, outputBuffer, NULL);
        outputBuffer = join(outputBuffer, "\n", NULL);
        return outputBuffer;
}*/

int main(int argc, char *argv[]) {
    // READ F7.csv
    // argv[1] is the relative path to the input file and it is passes as a keyboard argument
    int fileSize = get_file_size(argv[1]);
    // allocate buffer to read file of size fileSize + 1(space for /0)
    char *inputBuffer = malloc(sizeof(char) * fileSize + 1);
    if (inputBuffer == NULL) {
        ErrExit("malloc");
    }
    read_file(inputBuffer, argv[1], fileSize);
    int message_number = count_messages(inputBuffer, fileSize);
    hackler_struct *messages = parse_hackler_struct(inputBuffer, message_number);
    free(inputBuffer);


    // READ f8.csv
    char path_sender[] = "OutputFiles/F8.csv";
    fileSize = get_file_size(path_sender);
    // allocate buffer to read file of size fileSize + 1(space for /0)
    inputBuffer = malloc(sizeof(char) * fileSize + 1);
    if (inputBuffer == NULL)
        ErrExit("malloc");

    read_file(inputBuffer, path_sender, fileSize);
    message_number = count_messages(inputBuffer, fileSize);
    pid_struct *sender_messages = parse_pid_struct(inputBuffer, message_number);
    free(inputBuffer);

    // READ f9.csv
    char path_receiver[] = "OutputFiles/F9.csv";
    fileSize = get_file_size(path_receiver);
    // allocate buffer to read file of size fileSize + 1(space for /0)
    inputBuffer = malloc(sizeof(char) * fileSize + 1);
    if (inputBuffer == NULL)
        ErrExit("malloc");

    read_file(inputBuffer, path_receiver, fileSize);
    message_number = count_messages(inputBuffer, fileSize);
    pid_struct *receiver_messages = parse_pid_struct(inputBuffer, message_number);
    free(inputBuffer);
//    handle_signals(sender_messages, receiver_messages, messages, message_number);

    free(messages);
    free(sender_messages);
    free(receiver_messages);
    return 0;
}
