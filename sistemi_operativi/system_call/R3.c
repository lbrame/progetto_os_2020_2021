#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"
#include "fifo.h"

void send_message(Message_struct* message, int pipe)
{
    pid_t pid = fork();
    if(pid == 0) {
        sleep(message->DelS3);
        if(strcmp(message->IdReceiver, "R3") != 0) {
            write_pipe(pipe, message);
            printf("R3 sent id: %d\n", message->Id);
        }
        close_pipe(pipe);
        exit(0);
    }
}

int main(int argc, char * argv[]) {
    int pipe3_write = atoi(argv[0]);

    //Reading files from my_fifo.txt and saving them with the mechanism of S1
    int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDONLY);
    Message_struct *message = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_message = (Message_struct *) malloc(sizeof(Message_struct));
    if (message == NULL || last_message == NULL)
        ErrExit("malloc R3");
    ssize_t status;
    do{//Read until it returns 0 (EOF)
        // ROBA FIFO
        memcpy(last_message, message, sizeof(Message_struct));
        //using read_pipe as a reader also for fifo (they works the same way)
        status = read_pipe(fd_fifo, message);
        if(message->Id == last_message->Id)
            continue;
        send_message(message, pipe3_write);
        printf("status: %d\n", status);
    }while(status > 0);
    printf("R3 E' USCITO DAL WHILE\n");

    close_pipe(pipe3_write);
    printf("IO R3 HO FINITO IL IO LAVORO\n");
    return 0;
}
