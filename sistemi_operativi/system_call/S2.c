/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "defines.h"
#include <unistd.h>
#include "err_exit.h"
#include "pipe.h"
#include "semaphore.h"
#include "files.h"
#include "message_queue.h"
#include <sys/msg.h>
#include "shared_memory.h"
#include <stdbool.h>

int pipe1_read;
int pipe2_write;
bool added_delay = false;
bool remove_msg = false;
bool send_msg = false;

void send_message(Message_struct *message, int pipe, Message_struct *shmemPointer, int semaphore_array) {
    // come parametro verrà passato l'id del semaforo
    char *time_arrival = (char *) malloc(sizeof(char) * 8);
    char *time_departure = (char *) malloc(sizeof(char) * 8);
    time_arrival = getTime(time_arrival);

    if (added_delay) {
        sleep(message->DelS2 + 5);
        added_delay = false;
    } else if (remove_msg) {
        free(time_arrival);
        free(time_departure);
        remove_msg = false;
        return;
    } else if (send_msg) {
        send_msg = false;
    } else {
        sleep(message->DelS2);
    }

    time_departure = getTime(time_departure);
    char *outputBuffer = concatenate(message, time_arrival, time_departure);

    if ((strcmp(message->Type, "FIFO") == 0) || (strcmp(message->IdSender, "S2") != 0)) {
        write_pipe(pipe, message);
    } else if (strcmp(message->Type, "Q") == 0) {
        //sent with message queue
        int fd_queue = msgGet();
        msgSnd(fd_queue, outputBuffer);
        struct msqid_ds buf;
        if (msgctl(fd_queue, IPC_STAT, &buf) < 0) {
            ErrExit("msgctl");
        }
    } else if (strcmp(message->Type, "SH") == 0) {
        P(semaphore_array, 0);
        memcpy(shmemPointer, message, sizeof(Message_struct));
    }

    int fd = my_open("OutputFiles/F2.csv", O_WRONLY | O_APPEND);


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
    printf("S1: signal handler started\n");

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
            printf("S2: Caught SIGQUIT\n");
            send_msg = true;
            break;
        case SIGTERM:
            printf("S2: Caught SIGTERM\n");
//            close_pipe(pipe1_read);
//            close_pipe(pipe2_write);
            exit(0);
        default:
            printf("S2: Signal not valid\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    pipe1_read = atoi(argv[0]);
    pipe2_write = atoi(argv[1]);

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

    int semaphore_array = semGet(2);
    int shmemId = get_shmem(sizeof(Message_struct));
    Message_struct *shmemPointer = (Message_struct *) attach_shmem(shmemId);

    char *starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F2.csv", starter);

    Message_struct *message = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_message = (Message_struct *) malloc(sizeof(Message_struct));
    if (message == NULL || last_message == NULL)
        ErrExit("malloc S2");
    ssize_t status;
    do { // Read until it returns 0 (EOF)
        memcpy(last_message, message, sizeof(Message_struct));
        status = read_pipe(pipe1_read, message);
        if (message->Id == last_message->Id)
            continue;
        send_message(message, pipe2_write, shmemPointer, semaphore_array);
    } while (status > 0);

    close_pipe(pipe1_read);
    close_pipe(pipe2_write);
    free(message);
    free(last_message);
    detach_shmem((int *) shmemPointer);

    pause();
    return 0;
}
