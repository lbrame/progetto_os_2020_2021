/// @file S3.c
/// @brief Contiene l'implementazione del processo S3 chiamato da SenderManager.

#include "unistd.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "err_exit.h"
#include "fifo.h"

int main(int argc, char * argv[]) {
    printf("EHI SONO QUI\n");
    int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDWR);
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    return 0;
}