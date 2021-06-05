#include "defines.h"
#include "unistd.h"
#include "pipe.h"
#include "err_exit.h"
#include "semaphore.h"
#include "files.h"
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <signal.h>

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
    printf("R1: %d\n", getpid());

    pipe4_read = atoi(&argv[0][0]);
    if(signal(SIGTERM, sigHandler) == SIG_ERR) {
        ErrExit("R1, SIGTERM");
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

    printf("R1: %d\n", getpid());

    int semaphore_array = semGet(7);

    char *starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F6.csv", starter);

    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_content = (Message_struct *) malloc(sizeof(Message_struct));
    if (content == NULL || last_content == NULL)
        ErrExit("malloc S2");

    ssize_t status;
    do { // Read until it returns 0 (EOF)
        char *time_arrival = (char *) malloc(sizeof(char) * 8);
        char *time_departure = "-";
        memcpy(last_content, content, sizeof(Message_struct));
        status = read_pipe(pipe4_read, content);
        if (content->Id == last_content->Id) {
            continue;
        }
        time_arrival = getTime(time_arrival);
        sleep(content->DelS1);

        int fd = my_open("OutputFiles/F6.csv", O_WRONLY | O_APPEND);
        char *outputBuffer = concatenate(content, time_arrival, time_departure);

        P(semaphore_array, 4);
        my_write(fd, outputBuffer, strlen(outputBuffer));
        V(semaphore_array, 4);

        close(fd);
        free(time_arrival);
        free(time_departure);
    } while (status > 0);

    close_pipe(pipe4_read);
    free(content);
    free(last_content);

    // pause()
    scanf(NULL);
    pause();
    return 0;
}
