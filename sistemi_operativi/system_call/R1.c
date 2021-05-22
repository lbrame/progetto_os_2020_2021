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

int main(int argc, char * argv[]) {
    int pipe4_read = atoi(&argv[0][0]);
    int semaphore_array = semGet(8);
    int shmemId = get_shmem(sizeof(Message_struct));
    Message_struct *shmemPointer = (Message_struct *) attach_shmem(shmemId);

    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F6.csv", starter);

    Message_struct *message = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_message = (Message_struct *) malloc(sizeof(Message_struct));
    if (message == NULL || last_message == NULL)
        ErrExit("malloc R1");

    ssize_t status = 1;

    // 2 --> 1. fifo
    //   --> 2. shmem
    int endFlag = 2;

    do { //Read until it returns 0 (EOF)
        char* time_arrival = (char* )malloc(sizeof (char) * 8);
        char* time_departure = "-";

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
            P(semaphore_array, 7);
            // write to shmem
            memcpy(last_message, message, sizeof(Message_struct));
            memcpy(message, shmemPointer, sizeof(Message_struct));
            V(semaphore_array,0);
            if (message->Id == last_message->Id || strcmp(message->IdReceiver, "R1") != 0)
                continue;
            time_arrival = getTime(time_arrival);
            sleep(message->DelS1);

            int fd = my_open("OutputFiles/F6.csv", O_WRONLY | O_APPEND);
            char* outputBuffer = concatenate(message, time_arrival, time_departure);

            P(semaphore_array, 4);
            my_write(fd, outputBuffer, strlen(outputBuffer));
            V(semaphore_array, 4);

            close(fd);
            free(time_arrival);
            free(time_departure);
        } else
            endFlag--;

    } while (endFlag > 0);

    close_pipe(pipe4_read);
    free(message);
    free(last_message);
    return 0;
}
