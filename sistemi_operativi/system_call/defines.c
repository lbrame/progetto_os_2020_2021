/// @file defines.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche del progetto.

#include "defines.h"
#include <stdio.h>
#include "files.h"
#include <sys/stat.h>
#include "err_exit.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "pipe.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <signal.h>

/**
 * count the number of messages in the file
 * @param input the buffer of read text
 * @param fileSize the size of the buffer
 * @return the number of messages in the file
 * */
int count_messages(const char *input, int fileSize) {
    int counter = 0;
    for (int i = 1; i < fileSize + 1; i++) {
        //if after a letter or a number there's a \n, it's the beginning of a new message and the counter is incremented
        if (input[i] == '\n' || input[i] == '\0')
            counter++;
    }

    return counter - 1;
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
        return -1;
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
    int fd = my_open(rPath, O_RDONLY);
    ssize_t numRead;
    numRead = my_read(fd, inputBuffer, fileSize);
    if (numRead == -1) {
        ErrExit("read");
    }
    // insert terminator character
    inputBuffer[numRead] = '\0';
    close(fd);
}

/**
 * joins two strings
 * @param str1 the first string to be joined
 * @param str2 the second string to be joined
 * @param join_character the character to place between str1 and str2
 * @return a string composed of str1, join_character, str2
 */
char *join(char *str1, char *str2, char join_character) {
    //allocated the space needed for both the strings, the odd character and the \0
    int calloc_size = (int) (strlen(str1) + sizeof(join_character) + strlen(str2) + 1);
    // char *buffer = malloc(calloc_size);
    char *buffer = calloc(calloc_size, sizeof(join_character));
    // clean buffer
    // for (int i = 0; i < calloc_size; i++) buffer[i] = 0;
    //copied the content of str1 in buffer
    strcpy(buffer, str1);
    //if the strings are non zero -> calculate the position of join_character
    if (strcmp(str1, "") != 0 && strcmp(str2, "") != 0 && join_character) {
        buffer[calloc_size - strlen(str2) - 2] = join_character;
    }
    //append str2 to buffer
    strcat(buffer, str2);
    return buffer;
}

/**
 * generates the relative path to the output file
 * @param in_file_path the relative path to the input file
 * @return the relative path to the output file
 */
char *get_out_file_rpath(char *in_file_path) {
    char *fileDestination = strtok(in_file_path, "/");
    char *file_name_w_ext = strtok(NULL, "/");
    char *file_name = strtok(file_name_w_ext, ".");
    char *final_name = join(file_name, "out", '_');
    final_name = join(final_name, "csv", '.');
    final_name = join("OutputFiles", final_name, '/');
    return final_name;
}

/**
 * writes data to the output file
 * @param out_file_path the relative path to the output file
 * @param outputBuffer the buffer where is stored the data to write
 */
void write_file(char out_file_path[], char *outputBuffer) {
    // check if file exists
    if (access(out_file_path, F_OK) == 0) {
        // delete file
        int result = unlink(out_file_path);
        if (result == -1) {
            ErrExit("unlink");
        }
    }
    // create file and open it in write mode
    int fd = my_open_wmode(out_file_path, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, S_IRWXU);

    // write buffer to destination file
    ssize_t numWrite = my_write(fd, outputBuffer, strlen(outputBuffer));
    close(fd);
}


/**
 * converts integer value to string
 * @param val the value to convert
 * @return the input transformed into a string
 */
char *itoa(int val) {
    int buffer_dim = 0;
    int new = val;
    // count the number of digits
    while (new != 0) {
        new = new / 10;
        buffer_dim++;
    }
    char *buffer = malloc(sizeof(char) * buffer_dim);
    sprintf(buffer, "%d", val);
    buffer[buffer_dim] = '\0';
    return buffer;
}

/**
 *
 * @param fd
 * @param buffer
 * @return
 */
int read_line(int fd, char* buffer) {
    static int start = 0;
    int index = 0;
    off_t current = lseek(fd, start, SEEK_SET);
    if (current == -1)
        ErrExit("lseek");

    while (buffer[index - 1] != '\n' && buffer[index - 1] != EOF) {
        size_t numRead = my_read(fd, &buffer[index], 1);
        if (numRead == 0){
            buffer[index] = '\0';
            return 0;
        }
        index++;
    }
    start += index;
    buffer[index-1] = '\0';
    return 1;
}

/**
 *
 * @param inputBuffer
 * @param message_number
 * @return
 */
Message_struct *parse_message(char *inputBuffer) {
    static char *row_context;
    char *field_context;
    int field_counter = 0;
    Message_struct *message = malloc(sizeof(Message_struct));
    // iterate over the fields
    for (char *field_token = strtok_r(inputBuffer, ";", &field_context); field_token; field_token = strtok_r(NULL, ";", &field_context)) {
        switch (field_counter) {
            case 0:
                message->Id = atoi(field_token);
                break;
            case 1:
                strcpy(message->Message, field_token);
                break;
            case 2:
                strcpy(message->IdSender, field_token);
                break;
            case 3:
                strcpy(message->IdReceiver, field_token);
                break;
            case 4:
                message->DelS1 = atoi(field_token);
                break;
            case 5:
                message->DelS2 = atoi(field_token);
                break;
            case 6:
                message->DelS3 = atoi(field_token);
                break;
            case 7:
                strcpy(message->Type, field_token);
                break;
            default:
                ErrExit("parse_message");
        }
        field_counter++;
    }
    return message;
}

char* getTime(char* time_a) {
    time_t current_time;
    struct tm* time_info;
    char timeString[8];
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(timeString, 18, "%H:%M:%S", time_info);
    strcpy(time_a, timeString);
    /*
    for(int i = 0; i < 8; i++) {
        time_a[i] = timeString[i];
    }
     */

    return time_a;
}

/**
 *
 * @param message
 * @param counter
 * @param starter
 * @return
 */
char *concatenate(Message_struct *message, char* time_arrival, char* time_departure) {
    char *outputBuffer;
    char *old_outputBuffer;
    for (int field_n = 0; field_n <= 6; field_n++) {
        switch (field_n) {
            case 0:
                outputBuffer = itoa(message->Id);
                break;
            case 1:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, message->Message, ';');
                free(old_outputBuffer);
                break;
            case 2:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, message->IdSender, ';');
                free(old_outputBuffer);
                break;
            case 3:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, message->IdReceiver, ';');
                free(old_outputBuffer);
                break;
            case 4:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, time_arrival, ';');
                free(old_outputBuffer);
                break;
            case 5:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, time_departure, ';');
                free(old_outputBuffer);
                break;
            case 6:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, "\n", NULL);
                free(old_outputBuffer);
                break;
            default:
                ErrExit("Concatenate");
        }
    }
    return outputBuffer;
}

/**
 * join all messages preparing the text to be outputted to file
 * @param info_children a list containing the data of the children
 * @param counter the number of children of the process
 * @param starter the header of the file
 * @return the string to be outputted to file
 */
char *manager_concatenate(child_struct *info_children, int counter, char *starter) {
    char *outputBuffer;
    char *old_outputBuffer;
    for (int row = 0; row < counter; row++) {
        for (int field_n = 0; field_n <= 2; field_n++) {
            switch (field_n) {
                case 0:
                    if (row == 0) {
                        outputBuffer = join(info_children[row].sender_id, "", NULL);
                    } else {
                        old_outputBuffer = outputBuffer;
                        outputBuffer = join(outputBuffer, info_children[row].sender_id, NULL);
                        free(old_outputBuffer);
                    }
                    break;
                case 1:
                    old_outputBuffer = outputBuffer;
                    char *pid_s = itoa(info_children[row].pid);
                    outputBuffer = join(outputBuffer, pid_s, ';');
                    free(old_outputBuffer);
                    break;
                case 2:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, "\n", NULL);
                    free(old_outputBuffer);
                    break;
                default:
                    ErrExit("Manager Concatenate");
            }
        }
    }
    //Added the static string to outputBuffer
    outputBuffer = join(starter, outputBuffer, '\n');
    outputBuffer = join(outputBuffer, "\0", NULL);
    return outputBuffer;
}
