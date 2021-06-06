#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"
#include "semaphore.h"
#include "files.h"
#include "shared_memory.h"
#include <fcntl.h>
#include "message_queue.h"
#include <sys/msg.h>
#include <signal.h>
#include <stdbool.h>

int pipe3_read;
int pipe4_write;
bool added_delay = false;
bool remove_msg = false;
bool send_msg = false;

void send_message(Message_struct *message, int pipe) {
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

    if (strcmp(message->IdReceiver, "R2") != 0) {
        write_pipe(pipe, message);
    }
    time_departure = getTime(time_departure);

    int fd = my_open("OutputFiles/F5.csv", O_WRONLY | O_APPEND);
    char *outputBuffer = concatenate(message, time_arrival, time_departure);
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
    printf("R2: signal handler started\n");

    switch (sig) {
        case SIGUSR1:
            printf("Caught SIGUSR1\n");
            added_delay = true;
            break;
        case SIGUSR2:
            printf("Caught SIGUSR2\n");
            remove_msg = true;
            break;
        case SIGQUIT:
            printf("Caught SIGQUIT, reusing it\n");
            send_msg=true;
            break;
        case SIGTERM:
            printf("Caught SIGTERM\n");
            close_pipe(pipe3_read);
            close_pipe(pipe4_write);
            exit(0);
        default:
            printf("Signal not valid\n");
            break;
    }
}


int main(int argc, char *argv[]) {

    pipe3_read = atoi(argv[0]);
    pipe4_write = atoi(argv[1]);
    int semaphore_array = semGet(1);
    int shmemId = get_shmem(sizeof(Message_struct));
    //Queue file's descriptor
    int fd_queue = msgGet();
    Message_struct *shmemPointer = (Message_struct *) attach_shmem(shmemId);

    if(signal(SIGTERM, sigHandler) == SIG_ERR) {
        ErrExit("R2, SIGTERM");
    }
    if(signal(SIGUSR1, sigHandler) == SIG_ERR) {
        ErrExit("R1, SIGUSR1");
    }
    if(signal(SIGUSR2, sigHandler) == SIG_ERR) {
        ErrExit("R1, SIGUSR2");
    }
    if(signal(SIGQUIT, sigHandler) == SIG_ERR) {
        ErrExit("R1, SIGQUIT");
    }

    printf("R2: %d\n", getpid());

    char *starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F5.csv", starter);

    Message_struct *message = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_message = (Message_struct *) malloc(sizeof(Message_struct));
    if (message == NULL || last_message == NULL)
        ErrExit("malloc R2");

    ssize_t status = 1;

    // 2 --> 1. fifo
    //   --> 2. shmem
    int endFlag = 2;

    do { //Read until it returns 0 (EOF)
        if (status > 0) {
            memcpy(last_message, message, sizeof(Message_struct));
            
            status = read_pipe(pipe3_read, message);
            if (message->Id == last_message->Id)
                continue;
            if(strcmp(message->IdReceiver, "R2") != 0  && strcmp(message->Type, "Q") == 0) {
              write_pipe(pipe4_write, message);
              continue;
            }
            send_message(message, pipe4_write);
        } else
            endFlag--;

        // shmem
        if (strcmp(message->Message, "END") != 0) {
            // read from shmem
            memcpy(last_message, message, sizeof(Message_struct));
            memcpy(message, shmemPointer, sizeof(Message_struct));
            if (message->Id == last_message->Id )
                continue;
            else if (strcmp(message->IdReceiver, "R2") == 0)
                V(semaphore_array,0);
        } else
            endFlag--;

    } while (endFlag > 0);


    struct msqid_ds buf;
    if (msgctl(fd_queue, IPC_STAT, &buf) < 0)
        ErrExit("msgctl");



    close_pipe(pipe3_read);
    close_pipe(pipe4_write);
    free(message);
    free(last_message);
    pause();

    return 0;
}
