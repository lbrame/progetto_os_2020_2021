/// @file fifo.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione delle FIFO.

#include "unistd.h"
#include "err_exit.h"
#include "fifo.h"
#include "defines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "unistd.h"
#include "fifo.h"
#include "defines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

void generate_fifo(char *string) {
    int status_code = mkfifo(string, S_IRUSR | S_IWUSR | O_NONBLOCK);
    if (status_code == -1)
        ErrExit("Fifo creation");
}

int open_fifo(char *string, int type) {
    int fd = open(string, type);
    if (fd == -1)
        ErrExit("FIFO OPEN S3");
    return fd;
}

void close_fifo(int fd) {
    if (close(fd) == -1)
        ErrExit("CLOSE FIFO");
}
