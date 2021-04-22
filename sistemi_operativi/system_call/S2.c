/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "pipe.h"
#include "err_exit.h"

int main(int argc, char * argv[]) {
    int pipe1_read = atoi(argv[0]);
    int pipe2_write = atoi(argv[1]);

    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
    if (content == NULL)
        ErrExit("malloc S2");
    ssize_t status;
    do { // Read until it returns 0 (EOF)
        status = read_pipe(pipe1_read, content);
        printf("S2 read: %d\n", content->Id);
        sleep(content->DelS2);
        if((strcmp(content->Type, "FIFO") == 0) || (strcmp(content->IdSender, "S2") != 0)) {
            write_pipe(pipe2_write, content);
            printf("S2 write: %d\n", content->Id);
        }
        else if(strcmp(content->Type, "Q") == 0) {
            // TODO send with queue
        }
        else if(strcmp(content->Type, "SH") == 0) {
            // TODO send with shared memory
        }
    } while (status > 0);

    close_pipe(pipe1_read);
    close_pipe(pipe2_write);
    sleep(2);
    return 0;
}