/// @file fifo.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione delle FIFO.

#include "unistd.h"
#include "err_exit.h"
#include "fifo.h"

void generate_fifo(int fd)
{
    if(fd == -1)
        ErrExit("Fifo creation");
}

void close_fifo(int fd)
{
    if(close(fd) == -1)
        ErrExit("CLOSE FIFO");
}