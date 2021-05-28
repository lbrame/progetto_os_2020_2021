/// @file S1.c
/// @brief Contiene l'implementazione del processo S1 chiamato da SenderManager.

#include "unistd.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "err_exit.h"
#include "pipe.h"
#include "semaphore.h"
#include "files.h"
#include "message_queue.h"
#include <sys/msg.h>


void send_message(Message_struct* message, int pipe) {
    pid_t pid = fork();
    // come parametro verrà passato l'id del semaforo
    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = (char* )malloc(sizeof (char) * 8);
    if(pid == 0) {
        int semaphore_array = semGet(7);

        time_arrival = getTime(time_arrival);
        sleep(message->DelS1);

        time_departure = getTime(time_departure);
        char* outputBuffer = concatenate(message, time_arrival, time_departure);

        if ((strcmp(message->Type, "FIFO") == 0) || (strcmp(message->IdSender, "S1") != 0)) {
            write_pipe(pipe, message);
        } else if (strcmp(message->Type, "Q") == 0) {
            //sent with message queue
            int fd_queue = msgGet();
            msgSnd(fd_queue, outputBuffer);
            struct msqid_ds buf;
            if(msgctl(fd_queue, IPC_STAT, &buf) < 0)
                ErrExit("msgctl");
            printf("S1 sent message to queue\n");
        } else if (strcmp(message->Type, "SH") == 0) {
            // TODO send with shared memory
        }

        int fd = my_open("OutputFiles/F1.csv", O_WRONLY | O_APPEND);

        P(semaphore_array, 1);
        my_write(fd, outputBuffer, strlen(outputBuffer));
        V(semaphore_array, 1);

        close(fd);
        free(time_arrival);
        free(time_departure);
        close_pipe(pipe);
        exit(0);
    }
}


int main(int argc, char * argv[]) {
    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F1.csv", starter);
    char *rPath = argv[0];
    int pipe1_write = atoi(argv[1]);
    int fd = my_open(rPath, O_RDONLY);
    // suppose each of the 8 fields has a maximum size of 50bytes
    char *buffer = (char *) malloc(8 * 50);
    if (buffer == NULL)
        ErrExit("malloc S1");
    int row_status = read_line(fd, buffer);
    while (row_status > 0) {
        row_status = read_line(fd, buffer);
        Message_struct *message = parse_message(buffer);
        send_message(message, pipe1_write);
    }
    free(buffer);
    close(fd);
    close_pipe(pipe1_write);
    //@TODO remove debug texts
    printf("S1 ha finito\n");

    return 0;
}
