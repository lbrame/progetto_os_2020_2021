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

    char *content = (char *) malloc(8 * 50);
    char* content_copy = (char*) malloc(8 * 50);
    if (content == NULL || content_copy == NULL)
        ErrExit("malloc S2");
    ssize_t status;
    do { // Read until it returns 0 (EOF)
        status = read_pipe(pipe1_read, content);
        strcpy(content_copy, content);
        Message_struct* message = parse_message(content);
        sleep(message->DelS2);

//        if(strcmp(message->Type, "Q") == 0) {
//            // TODO send with queue
//        }
//        else if(strcmp(message->Type, "SH") == 0) {
//            // TODO send with shared memory
//        }
//        else if(strcmp(message->Type, "FIFO") == 0)
//            write_pipe(pipe2_write, content_copy);
    } while (status > 0);

    close_pipe(pipe1_read);
    close_pipe(pipe2_write);
    sleep(2);
    return 0;
}