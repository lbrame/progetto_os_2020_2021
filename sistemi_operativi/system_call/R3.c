#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"

int main(int argc, char * argv[]) {
    // Reading of F0
    // TODO remove after merge
    char* rPath = "InputFiles/F0_test.csv";
    // get fd of the pipe
    int pipe4_write = atoi(argv[0]);
    // TODO remove after merge from here
    int fd = open(rPath, O_RDONLY);
    if (fd == -1)
        ErrExit("open");
    // suppose each of the 8 fields has a maximum size of 50bytes
    char *buffer = (char *) malloc(8 * 50);
    if (buffer == NULL)
        ErrExit("malloc S1");
    int row_status = read_line(fd, buffer);
    // TODO remove after merge until here
    while (row_status > 0) {
        // TODO remove after merge
        row_status = read_line(fd, buffer);
        Message_struct *message = parse_message(buffer);
        // TODO read fifo
        sleep(message->DelS1);
        if(strcmp(message->IdReceiver, "R3") != 0) {
//            write_pipe(pipe4_write, message);
            printf("sent message %d\n", message->Id);
        }
    }

    free(buffer);
    close(fd);
    close_pipe(pipe4_write);
    sleep(3);
    return 0;
}