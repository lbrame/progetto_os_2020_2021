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

void send_message(Message_struct *message, int pipe, char* queue_buffer) {
    char *time_arrival = (char *) malloc(sizeof(char) * 8);
    char *time_departure = (char *) malloc(sizeof(char) * 8);
    time_arrival = getTime(time_arrival);
    if (message != NULL){
      sleep(message->DelS3);
      if (strcmp(message->IdReceiver, "R3") != 0) {
          write_pipe(pipe, message);
      }
      time_departure = getTime(time_departure);
      char *outputBuffer = concatenate(message, time_arrival, time_departure);
    } else {
      outputBuffer = queue_buffer
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

            break;
        case SIGUSR2:
            printf("Caught SIGUSR2\n");
            break;
        case SIGQUIT:
            printf("Caught SIGQUIT, reusing it\n");
            break;
        case SIGTERM:
            printf("Caught SIGTERM\n");
            close_pipe(pipe3_write);
            exit(0);
        default:
            printf("Signal not valid\n");
            break;
    }
}

int main(int argc, char * argv[]) {
    pipe3_write = atoi(argv[0]);
    int semaphore_array = semGet(1);
    int shmemId = get_shmem(sizeof(Message_struct));
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

    // 2 --> 1. fifo
    //   --> 2. shmem
    int endFlag = 2;

    do { // Read until it returns 0 (EOF)
        if (status > 0) {
            memcpy(last_message, message, sizeof(Message_struct));
            // using read_pipe as a reader also for fifo (they works the same way)
            status = read_pipe(fd_fifo, message);
            if (message->Id == last_message->Id)
                continue;
            send_message(message, pipe3_write, NULL);
        } else
            endFlag--;

        // shmem
        if (strcmp(message->Message, "END") != 0) {
            // write to shmem
            memcpy(last_message, message, sizeof(Message_struct));
            memcpy(message, shmemPointer, sizeof(Message_struct));
            printf("R3 shmem: %s\n", message->Message);
            if (message->Id == last_message->Id )
                continue;
            else if (strcmp(message->IdReceiver, "R3") == 0)
                V(semaphore_array,0);
        } else
            endFlag--;
      
      // MSG QUEUE
      char* outputbuffer = msgRcv(fd_queue, outputbuffer);
      if(outputbuffer == NULL || buf.msg_qnum == 0)
           break;
      char* tmp;
      if((tmp =  strstr(outputbuffer, "R3")) != NULL) {
        //send message to write in outputbuffer
        send_message(NULL, 0, outputbuffer);
      }
      else {
            Message_struct* queue_message = parse_message(outputbuffer);
            //send without printing
            write_pipe(pipe3_write, queue_message);
        }

    } while (endFlag > 0);
  
    struct msqid_ds buf;
    if (msgctl(fd_queue, IPC_STAT, &buf) < 0)
        ErrExit("msgctl");

    memcpy(last_message, message, sizeof(Message_struct));
    close_pipe(pipe3_write);
    free(message);
    free(last_message);
    pause();
    return 0;
}
