/// @file defines.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche del progetto.

#include "defines.h"
#include <stdio.h>
#include <sys/stat.h>
#include "err_exit.h"
#include <unistd.h>
#include <fcntl.h>

/**
 * count the number of messages in the file
 * @param input the buffer of read text
 * @param fileSize the size of the buffer
 * @return the number of messages in the file
 * */
int count_messages(char* input, int fileSize) {
    int counter = 0;
    for (int i = 1; i < fileSize + 1; i++) {
        if (input[i] == '\n' && input[i-1] >= '0' && input[i-1] <= 'z')
            counter++;
    }
    return counter-1;
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