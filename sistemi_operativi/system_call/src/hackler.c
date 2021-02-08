/// @file client.c
/// @brief Contiene l'implementazione del client.

#include <stdio.h>
#include <sys/stat.h>
#include "err_exit.h"
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>

typedef struct {
    char* id;
    char* message;
    char* id_sender;
    char* id_receiver;
    char* time_arrival;
    char* time_departure;
} hackler_struct;


/**
 * count the number of messages in the file
 * @param input the buffer of read text
 * @param fileSize the size of the buffer
 * @return the number of messages in the file
 * */
int count_messages(char *input, int fileSize) {
    int counter = 0;
    for (int i = 0; i < fileSize + 1; i++) {
        if (input[i] == '\n')
            counter++;
    }
    return counter;
}


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
    printf("time_departure: %s\n\n", m.time_arrival);
}

/**
 * parse the read text to create a list of structures
 * @param inputBuffer the buffer of read text
 * @param fileSize the size of the buffer
 * @return a pointer to a list of hackler_struct
 */
hackler_struct* parse_file(char *inputBuffer, int fileSize) {
    hackler_struct *messages = malloc(count_messages(inputBuffer, fileSize) * sizeof(hackler_struct));
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
            print_message(messages[row_counter-1]);
        }
        row_counter++;
    }

    return messages;
}

/**
 * get the size of the file
 * @param rPath the relative path to the file that need to be read
 * @return the number of character contained into the file
 */
int get_file_size(char *rPath) {
    // Get file fileSize
    struct stat st;
    int statStatus = stat(rPath, &st);
    if (statStatus == -1) {
        ErrExit("stat");
    }
    return st.st_size;
}

/**
 * read the file and put read data into a buffer
 * @param inputBuffer the buffer where to store the read data
 * @param rPath the relative path to the file that need to be read
 * @param fileSize the size of file
 */
void read_file(char *inputBuffer, char *rPath, int fileSize) {
    int fd = open(rPath, O_RDONLY);
    if (fd == -1)
        ErrExit("open");
    ssize_t numRead;
    numRead = read(fd, inputBuffer, fileSize);
    if (numRead == -1) {
        ErrExit("read");
    }
    // insert terminator character
    inputBuffer[numRead] = '\0';
    close(fd);
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
    hackler_struct* messages = parse_file(inputBuffer, fileSize);
    return 0;
}