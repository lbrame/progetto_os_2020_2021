/// @file S1.c
/// @brief Contiene l'implementazione del processo S1 chiamato da SenderManager.

#include "unistd.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "err_exit.h"
#include "pipe.h"
#include "semaphore.h"
#include "files.h"


/**
 *
 * @param message
 * @param counter
 * @param starter
 * @return
 */
char *concatenate(Message_struct *message, char* time_arrival, char* time_departure) {
    char *outputBuffer;
    char *old_outputBuffer;
    for (int field_n = 0; field_n <= 6; field_n++) {
        switch (field_n) {
            case 0:
                outputBuffer = join(itoa(message->Id), "", NULL);
                break;
            case 1:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, message->Message, ';');
                free(old_outputBuffer);
                break;
            case 2:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, message->IdSender, ';');
                free(old_outputBuffer);
                break;
            case 3:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, message->IdReceiver, ';');
                free(old_outputBuffer);
                break;
            case 4:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, time_arrival, ';');
                free(old_outputBuffer);
                break;
            case 5:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, time_departure, ' ');
                free(old_outputBuffer);
                break;
            case 6:
                old_outputBuffer = outputBuffer;
                outputBuffer = join(outputBuffer, "\n", NULL);
                free(old_outputBuffer);
                break;
            default:
                ErrExit("Concatenate");
        }
    }
    return outputBuffer;
}


void send_message(Message_struct* message, int pipe)
{
    pid_t pid = fork();
    //come parametro verrà passato l'id del semaforo
    char* time_arrival = (char* )malloc(sizeof (char) * 8);
    char* time_departure = (char* )malloc(sizeof (char) * 8);
    if(pid == 0) {
        int semaphore_array = semGet(2);
        printf("dopo semget\n");
        printSem(7, semaphore_array);


        time_arrival = getTime(time_arrival);
        printf("Time arrival: %s\n", time_arrival);
        sleep(message->DelS1);
        if ((strcmp(message->Type, "FIFO") == 0) || (strcmp(message->IdSender, "S1") != 0)) {
            write_pipe(pipe, message);
            printf("S1 sent id: %d\n", message->Id);
        } else if (strcmp(message->Type, "Q") == 0) {
            // TODO send with queue
        } else if (strcmp(message->Type, "SH") == 0) {
            // TODO send with shared memory
        }
        time_departure = getTime(time_departure);
        printf("Time departure: %s\n", time_departure);

        int fd = my_open("OutputFiles/F1.csv", O_WRONLY);
        char* outputBuffer = concatenate(message, time_arrival, time_departure);
        printf("outputBuffer\n%s", outputBuffer);
        P(semaphore_array, 1);
        my_write(fd, outputBuffer, strlen(outputBuffer));
        printf("outputBuffer\n%s", outputBuffer);
        V(semaphore_array, 1);
        printf("uscito dalla sezione critica\n");
        printSem(1, semaphore_array);
        close(fd);
        free(time_arrival);
        free(time_departure);
        close_pipe(pipe);
        exit(0);
    }
}


int main(int argc, char * argv[]) {
    char* starter = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture\n";
    write_file("OutputFiles/F1.csv", starter);
    char *rPath = argv[0];
    int pipe1_write = atoi(argv[1]);
    int fd = my_open(rPath, O_RDONLY);
    // suppose each of the 8 fields has a maximum size of 50bytes
    char *buffer = (char *) malloc(8 * 50);
    if (buffer == NULL)
        ErrExit("malloc S1");
    int row_status = read_line(fd, buffer);
    while (row_status > 0) {
        row_status = read_line(fd, buffer);
        Message_struct *message = parse_message(buffer);
        send_message(message, pipe1_write);
    }
    free(buffer);
    close(fd);
    close_pipe(pipe1_write);
    return 0;
}
