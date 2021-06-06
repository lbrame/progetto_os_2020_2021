#include "defines.h"
#include "unistd.h"
#include "pipe.h"
#include "err_exit.h"
#include "semaphore.h"
#include "files.h"
#include "shared_memory.h"
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include "message_queue.h"
#include <sys/msg.h>

int pipe4_read;

/**
 * Signal handler
 * @param sig
 */
void sigHandler(int sig) {
    printf("R1: signal handler started\n");

    switch (sig) {
        case SIGUSR1:
            printf("Caught SIGUSR1\n");
            break;
        case SIGUSR2:
            printf("Caught SIGUSR2\n");
            break;
        case SIGQUIT:
            printf("Caught SIGQUIT, reusing it\n");
            break;
        case SIGTERM:
            printf("Caught SIGTERM\n");
            close_pipe(pipe4_read);
            exit(0);
        default:
            printf("Signal not valid\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    pipe4_read = atoi(&argv[0][0]);

    if (signal(SIGTERM, sigHandler) == SIG_ERR) {
        ErrExit("R1, SIGTERM");
    }
    if (signal(SIGUSR1, sigHandler) == SIG_ERR) {
        ErrExit("R1, SIGUSR1");
    }
    if (signal(SIGUSR2, sigHandler) == SIG_ERR) {
        ErrExit("R1, SIGUSR2");
    }
    if (signal(SIGQUIT, sigHandler) == SIG_ERR) {
        ErrExit("R1, SIGQUIT");
    }

    printf("R1: %d\n", getpid());

    int semaphore_array = semGet(1);
    int shmemId = get_shmem(sizeof(Message_struct));
    int fd_queue = msgGet();

    Message_struct *shmemPointer = (Message_struct *) attach_shmem(shmemId);


    char *starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F6.csv", starter);

    struct msqid_ds buf;
    if (msgctl(fd_queue, IPC_STAT, &buf) < 0)
        ErrExit("msgctl");

    Message_struct *message = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_message = (Message_struct *) malloc(sizeof(Message_struct));
    if (message == NULL || last_message == NULL)
        ErrExit("malloc R1");


    ssize_t status = 1;

    // 2 --> 1. fifo
    //   --> 2. shmem
    int endFlag = 2;

    do { //Read until it returns 0 (EOF)
        char *time_arrival = (char *) malloc(sizeof(char) * 8);
        char *time_departure = "-";

        // read pipe
        if (status > 0) {
            memcpy(last_message, message, sizeof(Message_struct));
            // using read_pipe as a reader also for fifo (they works the same way)
            status = read_pipe(pipe4_read, message);
            if (message->Id == last_message->Id)
                continue;
        } else
            endFlag--;

        // shmem
        if (strcmp(message->Message, "END") != 0) {
            // write to shmem
            memcpy(last_message, message, sizeof(Message_struct));
            memcpy(message, shmemPointer, sizeof(Message_struct));
            if (message->Id == last_message->Id)
                continue;
            else if (strcmp(message->IdReceiver, "R1") == 0)
                V(semaphore_array,0);
            time_arrival = getTime(time_arrival);
            sleep(message->DelS1);

            int fd = my_open("OutputFiles/F6.csv", O_WRONLY | O_APPEND);
            char *outputBuffer = concatenate(message, time_arrival, time_departure);
            my_write(fd, outputBuffer, strlen(outputBuffer));

            close(fd);
            free(time_arrival);
            free(time_departure);
        } else
            endFlag--;

    } while (endFlag > 0);

    close_pipe(pipe4_read);
    free(message);
    free(last_message);
    pause();
    return 0;
}
