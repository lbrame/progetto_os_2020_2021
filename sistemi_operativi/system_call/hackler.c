/// @file client.c
/// @brief Contiene l'implementazione del client.

#include "err_exit.h"
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
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
 * parse the read text to create a list of structures
 * @param inputBuffer the buffer of read text
 * @param fileSize the size of the buffer
 * @return a pointer to a list of hackler_struct
 */
hackler_struct *parse_file(char *inputBuffer, int message_number) {
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
 * join all messages preparing the text to be outputted to file
 * @param messages a list containing all messages read from the file which are encapsulated into a structure
 * @param starter the starting string containing field names
 * @param message_number the number of messages contained into the input file
 * @return the string to be outputted to file
 */
char *concatenation(hackler_struct *messages, char *starter, int message_number) {
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
}

int main(int argc, char *argv[]) {
    struct stat sb;
    // If ./OutputFiles does not exist, create it
    if (stat("OutputFiles", &sb) != 0)
        mkdir("OutputFiles", S_IRWXU);

    // argv[1] is the relative path to the input file and it is passes as a keyboard argument
    int fileSize = get_file_size(argv[1]);
    // allocate buffer to read file of size fileSize + 1(space for /0)
    char *inputBuffer = malloc(sizeof(char) * fileSize + 1);
    if (inputBuffer == NULL) {
        ErrExit("malloc");
    }
    read_file(inputBuffer, argv[1], fileSize);
    int message_number = count_messages(inputBuffer, fileSize);
    hackler_struct* messages = parse_file(inputBuffer, message_number);
    free(inputBuffer);

    char firstRow[] = "Id;Message;Id_Sender;Id_Receiver;Time_arrival;Time_departure\n";
    char* BUFFER = concatenation(messages, firstRow, message_number);
    char* r_path = get_out_file_rpath(argv[1]);
    write_file(r_path, BUFFER);

    free(BUFFER);
    free(messages);

    sleep(2);

    return 0;
}
