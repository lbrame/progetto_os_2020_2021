#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"
#include "fifo.h"
#include "semaphore.h"
#include "files.h"
#include "message_queue.h"
#include <sys/msg.h>

void send_message(Message_struct* message, int pipe)
{
    pid_t pid = fork();

    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = (char* )malloc(sizeof (char) * 8);

    if(pid == 0) {
        int semaphore_array = semGet(7);

        time_arrival = getTime(time_arrival);
        sleep(message->DelS3);
        if(strcmp(message->IdReceiver, "R3") != 0) {
            write_pipe(pipe, message);
        }
        time_departure = getTime(time_departure);

        int fd = my_open("OutputFiles/F4.csv", O_WRONLY | O_APPEND);
        char* outputBuffer = concatenate(message, time_arrival, time_departure);

        P(semaphore_array, 4);
        my_write(fd, outputBuffer, strlen(outputBuffer));
        V(semaphore_array, 4);

        close(fd);
        free(time_arrival);
        free(time_departure);

        close_pipe(pipe);
        exit(0);
    }
}

int main(int argc, char * argv[]) {
    int pipe3_write = atoi(argv[0]);

    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F4.csv", starter);

    //Reading files from my_fifo.txt and saving them with the mechanism of S1
    int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDONLY);

    //Queue file's descriptor
    int fd_queue = msgGet();

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
    }while(status > 0);

    struct msqid_ds buf;
    if (msgctl(fd_queue, IPC_STAT, &buf) < 0)
        ErrExit("msgctl");

   while(1) {
       printf("number of messages in queue before msgrcv %ld\n", buf.msg_qnum);
       char* outputbuffer = msgRcv(fd_queue, outputbuffer);
       printf("outputbuffer = %s\n", outputbuffer);
       printf("number of messages in queue after msgrcv %ld\n", buf.msg_qnum);
       if(outputbuffer == NULL || buf.msg_qnum == 0)
           break;
    }

   printf("AFTER WHILE\n");

    memcpy(last_message, message, sizeof(Message_struct));

    close_pipe(pipe3_write);
    return 0;
}
