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
#include "semaphore.h"
#include "files.h"
#include "message_queue.h"
#include <sys/msg.h>

void send_message(Message_struct* message)
{
    pid_t pid = fork();
    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = (char* )malloc(sizeof (char) * 8);
    if(pid == 0) {
        int semaphore_array = semGet(7);

        time_arrival = getTime(time_arrival);
        int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDWR);
        sleep(message->DelS3);

        time_departure = getTime(time_departure);
        int fd = my_open("OutputFiles/F3.csv", O_WRONLY | O_APPEND);
        char* outputBuffer = concatenate(message, time_arrival, time_departure);

        if(strcmp(message->Type, "FIFO") == 0){
            write_pipe(fd_fifo, message);
        }
        else if(strcmp(message->Type, "Q") == 0) {
            //sent with message queue
            int fd_queue = msgGet();
            msgSnd(fd_queue, outputBuffer);
            struct msqid_ds buf;
            if(msgctl(fd_queue, IPC_STAT, &buf) < 0)
                ErrExit("msgctl");
            printf("S3 sent message to queue\n");
        }
        else if(strcmp(message->Type, "SH") == 0) {
            // TODO send with shared memory
        }

        P(semaphore_array, 3);
        my_write(fd, outputBuffer, strlen(outputBuffer));
        V(semaphore_array, 3);

        close(fd);
        free(time_arrival);
        free(time_departure);

        close(fd_fifo);
        exit(0);
    }
}

int main(int argc, char * argv[]) {
    int pipe2_read = atoi(&argv[0][0]);

    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F3.csv", starter);

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
