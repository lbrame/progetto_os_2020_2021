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
#include "shared_memory.h"
#include <signal.h>
#include <stdbool.h>

int pipe2_read;
bool added_delay = false;
bool remove_msg = false;
bool send_msg = false;

void send_message(Message_struct *message, int fd_fifo, Message_struct *shmemPointer, int semaphore_array) {
    char *time_arrival = (char *) malloc(sizeof(char) * 8);
    char *time_departure = (char *) malloc(sizeof(char) * 8);
    time_arrival = getTime(time_arrival);

    if (added_delay) {
        sleep(message->DelS3 + 5);
        added_delay = false;
    } else if (remove_msg) {
        free(time_arrival);
        free(time_departure);
        remove_msg = false;
        return;
    } else if (send_msg) {
        send_msg = false;
    } else {
        sleep(message->DelS3);
    }

    time_departure = getTime(time_departure);
    char *outputBuffer = concatenate(message, time_arrival, time_departure);

    if (strcmp(message->Type, "FIFO") == 0) {
        write_pipe(fd_fifo, message);
    } else if (strcmp(message->Type, "Q") == 0) {
        //sent with message queue
        int fd_queue = msgGet();
        msgSnd(fd_queue, outputBuffer);
        struct msqid_ds buf;
        if (msgctl(fd_queue, IPC_STAT, &buf) < 0)
            ErrExit("msgctl");
    } else if (strcmp(message->Type, "SH") == 0) {
        P(semaphore_array, 0);
        memcpy(shmemPointer, message, sizeof(Message_struct));
    }

    int fd = my_open("OutputFiles/F3.csv", O_WRONLY | O_APPEND);

    my_write(fd, outputBuffer, strlen(outputBuffer));
    close(fd);
    free(time_arrival);
    free(time_departure);
}

/**
 * Signal handler
 * @param sig
 */
void sigHandler(int sig) {
    printf("S3: signal handler started\n");

    switch (sig) {
        case SIGUSR1:
            printf("S2: Caught SIGUSR1\n");
            added_delay = true;
            break;
        case SIGUSR2:
            printf("S2: Caught SIGUSR2\n");
            remove_msg = true;
            break;
        case SIGQUIT:
            printf("S2: Caught SIGQUIT, reusing it\n");
            send_msg = true;
            break;
        case SIGTERM:
            printf("S2: Caught SIGTERM\n");
            close_pipe(pipe2_read);
            exit(0);
        default:
            printf("S2: Signal not valid\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    pipe2_read = atoi(&argv[0][0]);

    if (signal(SIGTERM, sigHandler) == SIG_ERR) {
        ErrExit("S2, SIGTERM");
    }
    if (signal(SIGUSR1, sigHandler) == SIG_ERR) {
        ErrExit("S2, SIGUSR1");
    }
    if (signal(SIGUSR2, sigHandler) == SIG_ERR) {
        ErrExit("S2, SIGUSR2");
    }
    if (signal(SIGQUIT, sigHandler) == SIG_ERR) {
        ErrExit("S2, SIGQUIT");
    }

    int semaphore_array = semGet(1);
    int shmemId = get_shmem(sizeof(Message_struct));
    Message_struct *shmemPointer = (Message_struct *) attach_shmem(shmemId);
    int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDWR);

    char *starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F3.csv", starter);

    Message_struct *message = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_message = (Message_struct *) malloc(sizeof(Message_struct));
    if (message == NULL || last_message == NULL)
        ErrExit("malloc S3");

    ssize_t status;
    do { // Read until it returns 0 (EOF)
        memcpy(last_message, message, sizeof(Message_struct));
        status = read_pipe(pipe2_read, message);
        if (message->Id == last_message->Id) {
            continue;
        }
        send_message(message, fd_fifo, shmemPointer, semaphore_array);

    } while (status > 0);

    close_pipe(pipe2_read);
    close_pipe(fd_fifo);

    // write end message to SHMEM
    strcpy(message->Message, "END");
    // max int
    message->Id = 2147483647;

    free(message);
    free(last_message);
    detach_shmem((int *) shmemPointer);

    pause();
    return 0;
}
