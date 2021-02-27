/// @file S1.c
/// @brief Contiene l'implementazione del processo S1 chiamato da SenderManager.

#include "unistd.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>
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
S1_struct *parse_string(char *inputBuffer, int message_number) {
    S1_struct *messages = malloc(message_number * (int)sizeof(S1_struct));
    char *row_context;
    char *field_context;
    int row_counter = 0;

    // iterate over the rows
    for (char *row_token = strtok_r(inputBuffer, "\n", &row_context); row_token; row_token = strtok_r(NULL, "\n", &row_context)) {
        // row_counter at 0 is the first row which contains the field names
        if (row_counter != 0) {
            int field_counter = 0;
            S1_struct *message = malloc(sizeof(S1_struct));
            // iterate over the fields
            for (char *field_token = strtok_r(row_token, ";", &field_context); field_token; field_token = strtok_r(NULL, ";", &field_context)) {
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
            messages[row_counter-1] = *message;
        }
        row_counter++;
    }
    return messages;
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
    //Reading of F0
    char rPath[] = "InputFiles/F0.csv";
    //Getting the file size
    int file_size = get_file_size(rPath);
    //Memory allocated on the size of file_size
    char* inputBuffer = (char *)malloc(file_size);
    //Reading of the file -> read file implementes in define.c
    read_file(inputBuffer, rPath,file_size);
    int message_number = count_messages(inputBuffer, file_size);
    S1_struct* messages = parse_string(inputBuffer, message_number);
    char*starter = "Id;Message;IdSender;IdReceiver;DelS1;DelS2;DelS3;Type";
    //Creation of the outputBuffer that will be written in F1.csv
    char* outputBuffer = concatenate(messages, message_number, starter);
    write_file("OutputFiles/F1.csv", outputBuffer);
    //Free the memory allocated
    free(outputBuffer);
    free(messages);
    sleep(1);
    return 0;
}

