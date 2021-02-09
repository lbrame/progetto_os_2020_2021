/// @file client.c
/// @brief Contiene l'implementazione del client.

#include <stdio.h>
#include <sys/stat.h>
#include "err_exit.h"
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include "defines.h"

typedef struct {
    char* id;
    char* message;
    char* id_sender;
    char* id_receiver;
    char* time_arrival;
    char* time_departure;
} hackler_struct;


/**
 * DEBUG FUNCTION
 * prints the data contained into the parameter structure
 * @param m a structure containing message data
 * */
void print_message(hackler_struct m) {
    printf("id: %s\n", m.id);
    printf("message: %s\n", m.message);
    printf("id_sender: %s\n", m.id_sender);
    printf("id_receiver: %s\n", m.id_receiver);
    printf("time_arrival: %s\n", m.time_arrival);
    printf("time_departure: %s\n\n", m.time_departure);
}

/**
 * parse the read text to create a list of structures
 * @param inputBuffer the buffer of read text
 * @param fileSize the size of the buffer
 * @return a pointer to a list of hackler_struct
 */
hackler_struct *parse_file(char *inputBuffer, int fileSize, int message_number) {
    hackler_struct *messages = malloc(message_number * (int)sizeof(hackler_struct));
    char *row_context;
    char *field_context;
    int row_counter = 0;

    // iterate over the rows
    for (char *row_token = strtok_r(inputBuffer, "\n", &row_context); row_token; row_token = strtok_r(NULL, "\n", &row_context)) {
        // row_counter at 0 is the first row which contains the field names
        if (row_counter != 0) {
            int field_counter = 0;
            hackler_struct *message = malloc(sizeof(hackler_struct));
            // iterate over the fields
            for (char *field_token = strtok_r(row_token, ";", &field_context); field_token; field_token = strtok_r(NULL, ";", &field_context)) {
                switch (field_counter) {
                    case 0:
                        message->id = field_token;
                        break;
                    case 1:
                        message->message = field_token;
                        break;
                    case 2:
                        message->id_sender = field_token;
                        break;
                    case 3:
                        message->id_receiver = field_token;
                        break;
                    case 4:
                        message->time_arrival = field_token;
                        break;
                    case 5:
                        message->time_departure = field_token;
                        break;
                }
                field_counter++;
            }
            messages[row_counter-1] = *message;
            // TODO: remove
            print_message(messages[row_counter-1]);
        }
        row_counter++;
    }
    return messages;
}

/**
 * joins two strings
 * @param str1
 * @param str2
 * @param join_character
 * @return
 */
char* join (char* str1, char* str2, char join_character) {
    int malloc_size = (int) (strlen(str1)+sizeof(join_character)+strlen(str2)+sizeof(join_character));
    char* buffer = malloc(malloc_size);
    strcpy(buffer, str1);
    if (strcmp(str1, "") != 0 && strcmp(str2, "") != 0 && join_character) {
        buffer[malloc_size-strlen(str2)-2] = join_character;
    }
    strcat(buffer, str2);
    return buffer;
}

char *concatenation(hackler_struct *messages, char *starter, int message_number) {
    char* outputBuffer;
    char* old_outputBuffer;
    for(int row = message_number - 1; row >= 0; row--){
        for(int field_n = 0; field_n <= 6; field_n++) {
            switch (field_n) {
                case 0:
                    if(row == message_number-1) {
                        outputBuffer = join(messages[row].id, "", NULL);
                    } else {
                        old_outputBuffer = outputBuffer;
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
                    if (row > 0)
                        outputBuffer = join( outputBuffer, "\n", NULL);
                    else
                        outputBuffer = join( outputBuffer,"\0", ';');
                    free(old_outputBuffer);
                    break;
                }
            }
        }
        outputBuffer = join(starter, outputBuffer, NULL);
        outputBuffer = join(outputBuffer, "\n", NULL);
        printf("%s", outputBuffer);
        return outputBuffer;
}

int main(int argc, char *argv[]) {
    // argv[1] is the relative path to the input file and it is passes as a keyboard argument
    int fileSize = get_file_size(argv[1]);
    // allocate buffer to read file of size fileSize + 1(space for /0)
    char *inputBuffer = malloc(sizeof(char) * fileSize + 1);
    if (inputBuffer == NULL) {
        ErrExit("malloc");
    }
    read_file(inputBuffer, argv[1], fileSize);
    int message_number = count_messages(inputBuffer, fileSize);
    hackler_struct* messages = parse_file(inputBuffer, fileSize, message_number);
    free(inputBuffer);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    char firstRow[] = "Id;Message;Id_Sender;Id_Receiver;Time_arrival;Time_departure\n";
    char* BUFFER = concatenation(messages, firstRow, message_number);
    // TODO free all memory
    return 0;
}