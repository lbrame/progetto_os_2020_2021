/// @file pipe.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione delle PIPE.

#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "err_exit.h"
#include "pipe.h"
#include "defines.h"

void generate_pipe(int fd[]) {
    // checking if PIPE succeed
    if (pipe(fd) == -1)
        ErrExit("PIPE");
}

void close_pipe(int fd) {
    if (close(fd) == -1)
        ErrExit("close PIPE");
}

ssize_t read_pipe(int fd, Message_struct *content) {
    long size = sizeof(Message_struct);
    ssize_t status = read(fd, content, size);
    if (status == -1)
        ErrExit("read");
    return status;
}

void write_pipe(int fd,  Message_struct *buffer) {
    size_t size = sizeof(Message_struct);
    ssize_t numWrite = write(fd, buffer, size);
    if (numWrite == -1)
        ErrExit("write");
}