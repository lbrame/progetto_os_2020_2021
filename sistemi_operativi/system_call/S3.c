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
#include "shared_memory.h"
#include <signal.h>
int pipe2_read;

void send_message(Message_struct* message)
{
    pid_t pid = fork();
    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = (char* )malloc(sizeof (char) * 8);
    if(pid == 0) {
        int semaphore_array = semGet(8);
        int shmemId = get_shmem(sizeof(Message_struct));
        Message_struct* shmemPointer = (Message_struct*) attach_shmem(shmemId);

        time_arrival = getTime(time_arrival);
        int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDWR);
        sleep(message->DelS3);
        if(strcmp(message->Type, "FIFO") == 0){
            write_pipe(fd_fifo, message);
        }
        else if(strcmp(message->Type, "Q") == 0) {
            // TODO send with queue
        }
        else if(strcmp(message->Type, "SH") == 0) {
            P(semaphore_array, 0);
            memcpy(shmemPointer, message, sizeof(Message_struct));
            V(semaphore_array, 7);
        }

        int fd = my_open("OutputFiles/F3.csv", O_WRONLY | O_APPEND);
        char* outputBuffer = concatenate(message, time_arrival, time_departure);

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

/**
 * Signal handler
 * @param sig
 */
void sigHandler (int sig) {
    printf("S3: signal handler started\n");

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
            close_pipe(pipe2_read);
            exit(0);
        default:
            printf("S2: Signal not valid\n");
            break;
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
    st(content->Message, "END", 3);
    // max int
    content->Id = 2147483647;

    free(content);
    free(last_content);
    sleep(3);

    scanf(NULL);
    pause();
    return 0;
}
