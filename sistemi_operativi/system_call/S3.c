/// @file S3.c
/// @brief Contiene l'implementazione del processo S3 chiamato da SenderManager.

#include "defines.h"
#include "unistd.h"
#include "pipe.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
    int pipe2_read = atoi(&argv[0][0]);
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F3.csv", data);
    sleep(3);
    close_pipe(pipe2_read);
    return 0;

}