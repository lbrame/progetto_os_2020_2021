/// @file S1.c
/// @brief Contiene l'implementazione del processo S1 chiamato da SenderManager.

#include "unistd.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "err_exit.h"
#include "pipe.h"
#include "semaphore.h"
#include "files.h"
#include "shared_memory.h"

int pipe1_write;

void send_message(Message_struct* message, int pipe, Message_struct* shmemPointer, int semaphore_array) {
    // come parametro verrà passato l'id del semaforo
    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = (char* )malloc(sizeof (char) * 8);

    time_arrival = getTime(time_arrival);
    sleep(message->DelS1);
    if ((strcmp(message->Type, "FIFO") == 0) || (strcmp(message->IdSender, "S1") != 0)) {
        write_pipe(pipe, message);
    } else if (strcmp(message->Type, "Q") == 0) {
        // TODO send with queue
    } else if (strcmp(message->Type, "SH") == 0) {
        P(semaphore_array, 0);
        memcpy(shmemPointer, message, sizeof(Message_struct));
        V(semaphore_array, 7);
    }
    time_departure = getTime(time_departure);

    int fd = my_open("OutputFiles/F1.csv", O_WRONLY | O_APPEND);
    char* outputBuffer = concatenate(message, time_arrival, time_departure);

    my_write(fd, outputBuffer, strlen(outputBuffer));

    close(fd);
    free(time_arrival);
    free(time_departure);
}

/**
 * Signal handler
 * @param sig signal sent by the kernel
 * SIGTERM: terminate the process gracefully, properly closing all open IPCs
 * SIGUSR1: catch IncreaseDelay message (sent by hackler)
ch RemoveMsg message (sent by hackler)
 * SIGQUIT: catch SendMsg message (sent by hackler)
 */
void sigHandler (int sig) {
    printf("S1: signal handler started\n");

    switch (sig) {
        case SIGUSR1:
            printf("S1: Caught SIGUSR1\n");
            break;
        case SIGUSR2:
            printf("S1: Caught SIGUSR2\n");
            break;
        case SIGQUIT:
            printf("S1: Caught SIGQUIT\n");
            break;
        case SIGTERM:
            printf("S1: Caught SIGTERM\n");
            close_pipe(pipe1_write);
            exit(0);
        default:
            printf("S1: Signal not valid\n");
            break;
    }
}

int main(int argc, char * argv[]) {
    pipe1_write = atoi(argv[1]);
    if(signal(SIGTERM, sigHandler) == SIG_ERR) {
        ErrExit("S1, SIGTERM");
    }
    if(signal(SIGUSR1, sigHandler) == SIG_ERR) {
        ErrExit("S1, SIGUSR1");
    }
    if(signal(SIGUSR2, sigHandler) == SIG_ERR) {
        ErrExit("S1, SIGUSR2");
    }
    if(signal(SIGQUIT, sigHandler) == SIG_ERR) {
        ErrExit("S1, SIGQUIT");
    }
    int semaphore_array = semGet(8);
    int shmemId = get_shmem(sizeof(Message_struct));
    Message_struct* shmemPointer = (Message_struct*) attach_shmem(shmemId);

    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F1.csv", starter);
    char *rPath = argv[0];
    int fd_input = my_open(rPath, O_RDONLY);
    // suppose each of the 8 fields has a maximum size of 50bytes
    char *buffer = (char *) malloc(8 * 50);
    if (buffer == NULL)
        ErrExit("malloc S1");
    int row_status = read_line(fd_input, buffer);
    while (row_status > 0) {
        row_status = read_line(fd_input, buffer);
        Message_struct *message = parse_message(buffer);
        send_message(message, pipe1_write, shmemPointer, semaphore_array);
    }
    free(buffer);
    close(fd_input);
    close_pipe(pipe1_write);
    detach_shmem((int *) shmemPointer);
    pause();
    return 0;
}
