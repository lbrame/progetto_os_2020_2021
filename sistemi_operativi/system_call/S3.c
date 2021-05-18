/// @file S3.c
/// @brief Contiene l'implementazione del processo S3 chiamato da SenderManager.

#include "pipe.h"
#include "err_exit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include <fcntl.h>
#include "fifo.h"
#include "unistd.h"

void send_message(Message_struct* message)
{
    pid_t pid = fork();
    if(pid == 0) {

        int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDWR);
        sleep(message->DelS3);
        if(strcmp(message->Type, "FIFO") == 0){
            write_pipe(fd_fifo, message);
            printf("S3 sent id: %d\n", message->Id);
        }
        else if(strcmp(message->Type, "Q") == 0) {
            // TODO send with queue
        }
        else if(strcmp(message->Type, "SH") == 0) {
            // TODO send with shared memory
        }
        close(fd_fifo);
        exit(0);
    }
}

int main(int argc, char * argv[]) {
    int pipe2_read = atoi(&argv[0][0]);

    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_content = (Message_struct *) malloc(sizeof(Message_struct));
    if (content == NULL || last_content == NULL)
        ErrExit("malloc S3");
    ssize_t status;
    do { // Read until it returns 0 (EOF)
        memcpy(last_content, content, sizeof(Message_struct));
        status = read_pipe(pipe2_read, content);
        if(content->Id == last_content->Id) {
            continue;
        }
        send_message(content);

    } while (status > 0);

    close_pipe(pipe2_read);

    free(content);
    free(last_content);
    sleep(3);

    return 0;

}
