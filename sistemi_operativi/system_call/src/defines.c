/// @file defines.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche del progetto.

#include "defines.h"
#include <stdio.h>
#include <sys/stat.h>
#include "err_exit.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>

/**
 * count the number of messages in the file
 * @param input the buffer of read text
 * @param fileSize the size of the buffer
 * @return the number of messages in the file
 * */
int count_messages(const char* input, int fileSize) {
    int counter = 0;
    for (int i = 1; i < fileSize + 1; i++) {
        //if after a letter or a number there's a \n, it's the beginning of a new message and the counter is incremented
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

/**
 * joins two strings
 * @param str1 the first string to be joined
 * @param str2 the second string to be joined
 * @param join_character the character to place between str1 and str2
 * @return a string composed of str1, join_character, str2
 */
char* join (char* str1, char* str2, char join_character) {
    //allocated the space needed for both the strings, the odd character and the \0
    int malloc_size = (int) (strlen(str1)+sizeof(join_character)+strlen(str2)+1);
    char* buffer = malloc(malloc_size);
    // clean buffer
    for (int i=0; i<malloc_size; i++) buffer[i]=0;
    //copied the content of str1 in buffer
    strcpy(buffer, str1);
    //if the strings are non zero -> calculate the position of join_character
    if (strcmp(str1, "") != 0 && strcmp(str2, "") != 0 && join_character) {
        buffer[malloc_size-strlen(str2)-2] = join_character;
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
char* get_out_file_rpath(char *in_file_path) {
    char* fileDestination = strtok(in_file_path, "/");
    char* file_name_w_ext = strtok(NULL, "/");
    char* file_name = strtok(file_name_w_ext, ".");
    char* final_name = join(file_name, "out", '_');
    final_name = join(final_name, "csv", '.');
    final_name = join("OutputFiles",final_name, '/');
    return final_name;
}

/**
 * writes data to the output file
 * @param out_file_path the relative path to the output file
 * @param outputBuffer the buffer where is stored the data to write
 */
void write_file(char out_file_path[], char* outputBuffer) {
    // check if file exists
    if (access(out_file_path, F_OK) == 0){
        // delete file
        int result = unlink(out_file_path);
        if (result == -1) {
            ErrExit("unlink");
        }
    }
    // create file and open it in write mode
    int fd = open(out_file_path, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, S_IRWXU);
    if (fd == -1)
        ErrExit("open");

    // write buffer to destination file
    ssize_t numWrite = write(fd, outputBuffer, strlen(outputBuffer));
    if (numWrite == -1)
        ErrExit("write");

    // insert terminator character
    outputBuffer[numWrite] = '\0';
    close(fd);
}

/**
 * converts integer value to string
 * @param val the value to convert
 * @return the input transformed into a string
 */
char* itoa(int val){
    int buffer_dim = 0;
    int new = val;
    // count the number of digits
    while(new != 0) {
        new = new/10;
        buffer_dim++;
    }
    char* buffer = malloc(sizeof(char) * buffer_dim);
    // itoa && assign values to buffer
    for (int i = 0; val > 0; i++) {
        new = val%10;
        buffer[i] = (char)(new + 48);
        val = val/10;
    }
    return buffer;
}
