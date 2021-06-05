#include "defines.h"
#include "unistd.h"
#include "pipe.h"
#include "err_exit.h"
#include "semaphore.h"
#include "files.h"
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include "message_queue.h"
#include <sys/msg.h>

void send_message(Message_struct* message, char* queue_buffer)
{
    pid_t pid = fork();

    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = "-";

    if(pid == 0)
    {
        int semaphore_array = semGet(7);
        int fd = my_open("OutputFiles/F6.csv", O_WRONLY | O_APPEND);
        char* outputBuffer;

        if(message != NULL)
        {
            time_arrival = getTime(time_arrival);
            outputBuffer = concatenate(message, time_arrival, time_departure);
        }else outputBuffer = queue_buffer;

        P(semaphore_array, 6);
        my_write(fd, outputBuffer, strlen(outputBuffer));
        V(semaphore_array, 6);

        close(fd);
        free(time_arrival);
        exit(0);
    }
}

int main(int argc, char * argv[]) {
    int pipe4_read = atoi(&argv[0][0]);
    int semaphore_array = semGet(7);

    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F6.csv", starter);

    int fd_queue = msgGet();

    Message_struct *message = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_message = (Message_struct *) malloc(sizeof(Message_struct));
    if (message == NULL || last_message == NULL)
        ErrExit("malloc S2");

    ssize_t status;
    do { // Read until it returns 0 (EOF)
        memcpy(last_message, message, sizeof(Message_struct));
        status = read_pipe(pipe4_read, message);
        if (message->Id == last_message->Id)
            continue;
        send_message(message, NULL);
    } while (status > 0);

    struct msqid_ds buf;
    if (msgctl(fd_queue, IPC_STAT, &buf) < 0)
        ErrExit("msgctl");


    close_pipe(pipe4_read);
    free(message);
    free(last_message);
    return 0;
}
