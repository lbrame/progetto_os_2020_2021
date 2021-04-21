/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "pipe.h"

int main(int argc, char * argv[]) {
    int pipe1_read = atoi(argv[0]);
    int pipe2_write = atoi(argv[1]);
    char *message;
    // TODO: find a way to read a message once it's written
    do { // Read until it returns 0 (EOF) or -1 (error)
        message = read_pipe(pipe1_read);
        printf("read content: %s", message);
    } while (message != NULL);
//    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
//    write_file("OutputFiles/F2.csv", data);
//    sleep(2);
    close_pipe(pipe1_read);
    close_pipe(pipe2_write);
    return 0;
}