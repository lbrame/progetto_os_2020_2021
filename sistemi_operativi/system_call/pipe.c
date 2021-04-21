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

void close_pipe(int fd[]) {
    if (pipe(&fd[0]) == -1)
        ErrExit("close PIPE READ");

    if (pipe(&fd[1]) == -1)
        ErrExit("close PIPE WRITE");
}

void read_pipe(int fd) {
    off_t size = get_file_size_from_fd(fd);
    char *content = (char *) malloc(size);
    if (content == NULL)
        ErrExit("malloc");

    ssize_t status = read(fd, content, size);
    if(status == -1)
        ErrExit("read");
}

void write_pipe(int fd, char *buffer) {
    size_t size = strlen(buffer);
    ssize_t numWrite = write(fd, buffer, size);
    if(numWrite == -1)
        ErrExit("write");
}