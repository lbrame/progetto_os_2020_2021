/// @file S1.c
/// @brief Contiene l'implementazione del processo S1 chiamato da SenderManager.

#include "unistd.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "err_exit.h"


typedef struct {
    char* Id;
    char* Message;
    char* IdSender;
    char* IdReceiver;
    char* DelS1;
    char* DelS2;
    char* DelS3;
    char* Type;
} S1_struct;

/**
 *
 * @param inputBuffer
 * @param message_number
 * @return
 */
S1_struct *parse_message(char *inputBuffer) {
    static char *row_context;
    char *field_context;
    int field_counter = 0;
    S1_struct *message = malloc(sizeof(S1_struct));
    // iterate over the fields
    for (char *field_token = strtok_r(inputBuffer, ";", &field_context); field_token; field_token = strtok_r(NULL, ";", &field_context)) {
        switch (field_counter) {
            case 0:
                message->Id = field_token;
                break;
            case 1:
                message->Message = field_token;
                break;
            case 2:
                message->IdSender = field_token;
                break;
            case 3:
                message->IdReceiver = field_token;
                break;
            case 4:
                message-> DelS1 = field_token;
                break;
            case 5:
                message->DelS2 = field_token;
                break;
            case 6:
                message->DelS3 = field_token;
                break;
            case 7:
                message->Type = field_token;
                break;
            default:
                ErrExit("parse_file");
        }
        field_counter++;
    }
    return message;
}

/**
 *
 * @param info_children
 * @param counter
 * @param starter
 * @return
 */
char *concatenate(S1_struct *info_children, int counter, char *starter) {
    char *outputBuffer;
    char *old_outputBuffer;
    for (int row = 0; row < counter; row++) {
        for (int field_n = 0; field_n <= 8; field_n++) {
            switch (field_n) {
                case 0:
                    if (row == 0) {
                        outputBuffer = join(info_children[row].Id, "", NULL);
                    } else {
                        old_outputBuffer = outputBuffer;
                        outputBuffer = join(outputBuffer, info_children[row].Id, NULL);
                        free(old_outputBuffer);
                    }
                    break;
                case 1:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, info_children[row].Message, ';');
                    free(old_outputBuffer);
                    break;
                case 2:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, info_children[row].IdSender, ';');
                    free(old_outputBuffer);
                    break;
                case 3:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, info_children[row].IdReceiver, ';');
                    free(old_outputBuffer);
                    break;
                case 4:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, info_children[row].DelS1, ';');
                    free(old_outputBuffer);
                    break;
                case 5:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, info_children[row].DelS2, ';');
                    free(old_outputBuffer);
                    break;
                case 6:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, info_children[row].DelS3, ';');
                    free(old_outputBuffer);
                    break;
                case 7:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, info_children[row].Type, ';');
                    free(old_outputBuffer);
                    break;
                case 8:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, "\n", NULL);
                    free(old_outputBuffer);
                    break;
                default:
                    ErrExit("Concatenate");
            }
        }
    }
    outputBuffer = join(starter, outputBuffer, '\n');
    outputBuffer = join(outputBuffer, "\0", NULL);
    return outputBuffer;
}


int main(int argc, char * argv[]) {
    // Reading of F0
    /*char* rPath = argv[0];
    int* pipe1 = (int *) argv[1];
    int fd = open(rPath, O_RDONLY);
    if (fd == -1)
        ErrExit("open S1");
    char *starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    char* row = read_line(fd);
    while (row != NULL) {
        row = read_line(fd);
        S1_struct *message = parse_message(row);
        //@TODO save messages in a buffer
        if (message == NULL)
            continue;
        free(row);
    }
    close(fd);*/
    //Creation of the outputBuffer that will be written in F1.csv
//    char* outputBuffer = concatenate(messages, message_number, starter);
//    write_file("OutputFiles/F1.csv", outputBuffer);
//    //Free the memory allocated
//    free(message)
    return 0;
}