/// @file pipe.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione delle PIPE.

#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "err_exit.h"
#include "pipe.h"
#include "defines.h"
#include "files.h"
#include <fcntl.h>
#include <sys/stat.h>

void generate_pipe(int fd[]) {
    // checking if PIPE creation is successful
    if (pipe(fd) == -1)
        ErrExit("PIPE");
    log_pipe();
}

void log_pipe() {
    printf("log pipe\n");
    // Check if F10.csv exists. If it does not, create it.
    struct stat sb;
    if (stat("OutputFiles/F10.csv", &sb) != 0) {
        char *f10_starter = "IPC;IDKey;Creator;CreationTime;DestructionTime\n";
        write_file("OutputFiles/F10.csv", f10_starter);
    }

    // Open F10.csv
    // int fd = my_open("OutputFiles/F10.csv", O_WRONLY | O_APPEND);
    // char *outputBuffer = strcat("this", "that");
    // my_write(fd, outputBuffer, strlen(outputBuffer));

    // close(fd);

}

void close_pipe(int fd) {
    if (close(fd) == -1)
        ErrExit("close PIPE");
}

ssize_t read_pipe(int fd, Message_struct *content) {
    long size = sizeof(Message_struct);
    ssize_t status = my_read(fd, content, size);

    return status;
}

void write_pipe(int fd, Message_struct *buffer) {
    size_t size = sizeof(Message_struct);
    ssize_t numWrite = my_write(fd, buffer, size);
}
