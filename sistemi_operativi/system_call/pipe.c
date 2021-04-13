/// @file pipe.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione delle PIPE.

#include <unistd.h>
#include "err_exit.h"
#include "pipe.h"

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
