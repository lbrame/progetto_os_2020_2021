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

char *read_pipe(int fd) {
    long size = 8 * 50;
    char *content = (char *) malloc(size);
    if (content == NULL)
        ErrExit("malloc read_pipe");

    ssize_t status = read(fd, content, size);
    printf("content: %s\n", content);
    if (status == -1)
        ErrExit("read");
    else if(status < 0)
        return NULL;
    return content;
}

void write_pipe(int fd, char *buffer) {
    size_t size = strlen(buffer);
    ssize_t numWrite = write(fd, buffer, size);
    if (numWrite == -1)
        ErrExit("write");
}