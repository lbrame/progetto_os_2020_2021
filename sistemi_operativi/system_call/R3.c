#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include "defines.h"
#include <unistd.h>
#include "err_exit.h"
#include "pipe.h"
#include "fifo.h"
#include "semaphore.h"
#include "files.h"
#include "message_queue.h"
#include <sys/msg.h>
#include "shared_memory.h"
#include <signal.h>

int pipe3_write;
bool added_delay = false;
bool remove_msg = false;
bool send_msg = false;

void send_message(Message_struct *message, int pipe, char *queue_buffer) {
    char *time_arrival = (char *) malloc(sizeof(char) * 8);
    char *time_departure = (char *) malloc(sizeof(char) * 8);
    time_arrival = getTime(time_arrival);

    char *outputBuffer;
    if (message != NULL) {


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

        if (strcmp(message->IdReceiver, "R3") != 0) {
            write_pipe(pipe, message);
        }
        time_departure = getTime(time_departure);
        outputBuffer = concatenate(message, time_arrival, time_departure);
    } else {
        outputBuffer = queue_buffer;
    }

    int fd = my_open("OutputFiles/F4.csv", O_WRONLY | O_APPEND);
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
            send_msg = true;
            break;
        case SIGTERM:
            printf("Caught SIGTERM\n");
//            close_pipe(pipe3_write);
            exit(0);
        default:
            printf("Signal not valid\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    pipe3_write = atoi(argv[0]);
    int fifoR2_R3 = open_fifo("OutputFiles/custom_fifo1.txt", O_RDONLY);
    int semaphore_array = semGet(3);
    int shmemId = get_shmem(sizeof(Message_struct));
    Message_struct *shmemPointer = (Message_struct *) attach_shmem(shmemId);

    if (signal(SIGTERM, sigHandler) == SIG_ERR) {
        ErrExit("R2, SIGTERM");
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

    char *starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F4.csv", starter);

    // Reading files from my_fifo.txt and saving them with the mechanism of S1
    int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDONLY);

    //Queue file's descriptor
    int fd_queue = msgGet();

    Message_struct *message = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_message = (Message_struct *) malloc(sizeof(Message_struct));

    if (message == NULL || last_message == NULL)
        ErrExit("malloc R3");

    ssize_t status = 1;
    ssize_t status_custom_fifo = 1;

    // 4 --> 1. fifo
    //   --> 2. shmem
    //   --> 3. queue
    //   --> 4. fifo custom
    int endFlag = 4;

    do { // Read until it returns 0 (EOF)
        if (status > 0) {
            memcpy(last_message, message, sizeof(Message_struct));
            // using read_pipe as a reader also for fifo (they works the same way)
            status = read_pipe(fd_fifo, message);
            if (message->Id != last_message->Id)
                send_message(message, pipe3_write, NULL);
        } else
            endFlag--;

        if (status_custom_fifo > 0) {
            memcpy(last_message, message, sizeof(Message_struct));
            // using read_pipe as a reader also for fifo (they works the same way)
            status_custom_fifo = read_pipe(fifoR2_R3, message);
            if (message->Id != last_message->Id)
                send_message(message, pipe3_write, NULL);
        } else
            endFlag--;

        memcpy(last_message, message, sizeof(Message_struct));
        memcpy(message, shmemPointer, sizeof(Message_struct));
        // shmem
        if (strcmp(message->Message, "END") != 0) {
            printf("R3 shmem: %s\n", message->Message);
            if (strcmp(message->IdReceiver, "R3") == 0 && message->Id != last_message->Id ) {
                V(semaphore_array, 0);
                send_message(message, pipe3_write, NULL);
            }
        } else
            endFlag--;

        // MSG QUEUE
        char *outputbuffer = msgRcv(fd_queue, outputbuffer);
        struct msqid_ds buf;
        if (outputbuffer == NULL || buf.msg_qnum == 0) {
            Message_struct *queue_message = parse_message(outputbuffer);
            if (strcmp(queue_message->Message, "R3") != 0) {
                //send without printing
                write_pipe(pipe3_write, queue_message);
            } else {
                // send message to the right receiver
                send_message(queue_message, pipe3_write, NULL);
            }
        } else {
            endFlag--;
        }


    } while (endFlag > 0);

    struct msqid_ds buf;
    if (msgctl(fd_queue, IPC_STAT, &buf) < 0)
        ErrExit("msgctl");


    close_pipe(pipe3_write);
    free(message);
    free(last_message);
    pause();
    return 0;
}
