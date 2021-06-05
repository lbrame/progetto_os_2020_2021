/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"
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
        sleep(message->DelS2);

        time_departure = getTime(time_departure);
        int fd = my_open("OutputFiles/F2.csv", O_WRONLY | O_APPEND);
        char* outputBuffer = concatenate(message, time_arrival, time_departure);

        if((strcmp(message->Type, "FIFO") == 0) || (strcmp(message->IdSender, "S2") != 0)) {
            write_pipe(pipe, message);
        }
        else if(strcmp(message->Type, "Q") == 0) {
            //sent with message queue
            int fd_queue = msgGet();
            msgSnd(fd_queue, outputBuffer);
            struct msqid_ds buf;
            if(msgctl(fd_queue, IPC_STAT, &buf) < 0)
                ErrExit("msgctl");
        }
        else if(strcmp(message->Type, "SH") == 0) {
            // TODO send with shared memory
        }

        P(semaphore_array, 2);
        my_write(fd, outputBuffer, strlen(outputBuffer));
        V(semaphore_array, 2);

        close(fd);
        free(time_arrival);
        free(time_departure);
        close_pipe(pipe);
        exit(0);
    }
}

int main(int argc, char * argv[]) {
    int pipe1_read = atoi(argv[0]);
    int pipe2_write = atoi(argv[1]);

    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F2.csv", starter);

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
