#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"
#include "semaphore.h"
#include "files.h"
#include <fcntl.h>

void send_message(Message_struct *message, int pipe) {
    pid_t pid = fork();

    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = (char* )malloc(sizeof (char) * 8);


    if (pid == 0) {
        int semaphore_array = semGet(7);
        time_arrival = getTime(time_arrival);

        sleep(message->DelS2);
        if (strcmp(message->IdReceiver, "R2") != 0) {
            write_pipe(pipe, message);
        }
        time_departure = getTime(time_departure);

        int fd = my_open("OutputFiles/F5.csv", O_WRONLY | O_APPEND);
        char* outputBuffer = concatenate(message, time_arrival, time_departure);

        P(semaphore_array, 5);
        my_write(fd, outputBuffer, strlen(outputBuffer));
        V(semaphore_array, 5);

        close(fd);
        free(time_arrival);
        free(time_departure);

        close_pipe(pipe);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int pipe3_read = atoi(argv[0]);
    int pipe4_write = atoi(argv[1]);

    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F5.csv", starter);

    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_content = (Message_struct *) malloc(sizeof(Message_struct));
    if (content == NULL || last_content == NULL)
        ErrExit("malloc S2");

    ssize_t status;
    do { // Read until it returns 0 (EOF)
        memcpy(last_content, content, sizeof(Message_struct));
        status = read_pipe(pipe3_read, content);
        if (content->Id == last_content->Id)
            continue;
        send_message(content, pipe4_write);
    } while (status > 0);

    close_pipe(pipe3_read);
    close_pipe(pipe4_write);
    free(content);
    free(last_content);
    return 0;
}