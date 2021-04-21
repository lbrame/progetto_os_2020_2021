/// @file S3.c
/// @brief Contiene l'implementazione del processo S3 chiamato da SenderManager.

#include "unistd.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "err_exit.h"

int main(int argc, char * argv[]) {
    int* pipe2 = (int *) argv[1];
    int* fifo = (int *) argv[2];
    int fd_fifo = open(fifo, O_WRONLY);
    //check open fifo
    if(fd_fifo == -1)
        ErrExit("FIFO OPEN");
    //open pipe 2 in readonly mode
    int fd = open(pipe2, O_RDONLY);
    //check open error
    if (fd == -1)
        ErrExit("open");
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F3.csv", data);
    sleep(3);
    return 0;
}