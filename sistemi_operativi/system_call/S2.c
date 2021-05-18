/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"

void send_message(Message_struct* message, int pipe)
{
    pid_t pid = fork();
    if(pid == 0) {
        
        sleep(message->DelS2);
        if((strcmp(message->Type, "FIFO") == 0) || (strcmp(message->IdSender, "S2") != 0)) {
            write_pipe(pipe, message);
            printf("S2 sent id: %d\n", message->Id);
        }
        else if(strcmp(message->Type, "Q") == 0) {
            // TODO send with queue
        }
        else if(strcmp(message->Type, "SH") == 0) {
            // TODO send with shared memory
        }
        close_pipe(pipe);
        exit(0);
    }
}

int main(int argc, char * argv[]) {
    int pipe1_read = atoi(argv[0]);
    int pipe2_write = atoi(argv[1]);

    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_content = (Message_struct *) malloc(sizeof(Message_struct));
    if (content == NULL || last_content == NULL)
        ErrExit("malloc S2");
    ssize_t status;
    do { // Read until it returns 0 (EOF)
        memcpy(last_content, content, sizeof(Message_struct));
        status = read_pipe(pipe1_read, content);
        if(content->Id == last_content->Id)
            continue;
        send_message(content, pipe2_write);
    } while (status > 0);

    close_pipe(pipe1_read);
    close_pipe(pipe2_write);
    free(content);
    free(last_content);
    return 0;
}
