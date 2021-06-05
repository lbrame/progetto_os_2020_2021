/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"
#include "semaphore.h"
#include "files.h"
#include "shared_memory.h"

int pipe1_read;
int pipe2_write;

void send_message(Message_struct* message, int pipe)
{
    pid_t pid = fork();
    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = (char* )malloc(sizeof (char) * 8);
    if(pid == 0) {
        int semaphore_array = semGet(8);
        int shmemId = get_shmem(sizeof(Message_struct));
        Message_struct* shmemPointer = (Message_struct*) attach_shmem(shmemId);

        time_arrival = getTime(time_arrival);
        sleep(message->DelS2);
        if((strcmp(message->Type, "FIFO") == 0) || (strcmp(message->IdSender, "S2") != 0)) {
            write_pipe(pipe, message);
        }
        else if(strcmp(message->Type, "Q") == 0) {
            // TODO send with queue
        }
        else if(strcmp(message->Type, "SH") == 0) {
            P(semaphore_array, 0);
            memcpy(shmemPointer, message, sizeof(Message_struct));
            V(semaphore_array, 7);
        }
        time_departure = getTime(time_departure);

        int fd = my_open("OutputFiles/F2.csv", O_WRONLY | O_APPEND);
        char* outputBuffer = concatenate(message, time_arrival, time_departure);

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

/**
 * Signal handler
 * @param sig
 */
void sigHandler (int sig) {
    printf("S1: signal handler started\n");

    switch (sig) {
        case SIGUSR1:
            printf("S2: Caught SIGUSR1\n");

            break;
        case SIGUSR2:
            printf("S2: Caught SIGUSR2\n");
            break;
        case SIGQUIT:
            printf("S2: Caught SIGQUIT, reusing it\n");
            break;
        case SIGTERM:
            printf("S2: Caught SIGTERM\n");
            close_pipe(pipe1_read);
            close_pipe(pipe2_write);
            exit(0);
        default:
            printf("S2: Signal not valid\n");
            break;
    }
}

int main(int argc, char * argv[]) {
    pipe1_read = atoi(argv[0]);
    pipe2_write = atoi(argv[1]);

    if(signal(SIGTERM, sigHandler) == SIG_ERR) {
        ErrExit("S1, SIGTERM");
    }

    printf("S2: %d\n", getpid());

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

    scanf(NULL);
    pause();
    return 0;
}
