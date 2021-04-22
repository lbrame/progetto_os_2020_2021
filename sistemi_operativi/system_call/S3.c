/// @file S3.c
/// @brief Contiene l'implementazione del processo S3 chiamato da SenderManager.

#include "defines.h"
#include "unistd.h"
#include "pipe.h"
#include "err_exit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {
    int pipe2_read = atoi(&argv[0][0]);
//    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
//    if (content == NULL)
//        ErrExit("malloc S3");
//    ssize_t status;
//    do { // Read until it returns 0 (EOF)
//        status = read_pipe(pipe2_read, content);
//        printf("S3 read: %d\n", content->Id);
//        sleep(content->DelS3);
//        if(strcmp(content->Type, "FIFO") == 0) {
//            // TODO send with fifo
//        }
//        else if(strcmp(content->Type, "Q") == 0) {
//            // TODO send with queue
//        }
//        else if(strcmp(content->Type, "SH") == 0) {
//            // TODO send with shared memory
//        }
//    } while (status > 0);

    close_pipe(pipe2_read);
    sleep(3);

    return 0;

}