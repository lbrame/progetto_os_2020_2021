#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"
#include "fifo.h"

int main(int argc, char * argv[]) {

    //Reading files from my_fifo.txt and saving them with the mechanism of S1
    int fd_fifo = open_fifo("OutputFiles/my_fifo.txt", O_RDWR);
    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_content = (Message_struct *) malloc(sizeof(Message_struct));
    if (content == NULL || last_content == NULL)
        ErrExit("malloc R3");
    ssize_t status;
    do{//Read until it returns 0 (EOF)
        memcpy(last_content, content, sizeof(Message_struct));
        //using read_pipe as a reader also for fifo (they works the same way)
        status = read_pipe(fd_fifo, content);
        if(content->Id == last_content->Id)
            continue;
        sleep(content->DelS2);
        if((strcmp(content->Type, "FIFO") == 0) || (strcmp(content->IdSender, "S2") != 0))
        {
            // TODO read the file to know what to do if the file was sent by fifo
        }
        else if(strcmp(content->Type, "Q") == 0) {
            // TODO send with queue
        }
        else if(strcmp(content->Type, "SH") == 0) {
            // TODO send with shared memory
        }
    }while(status > 0);

    /*
    // TODO remove after merge
    char* rPath = "InputFiles/F0_test.csv";
    // get fd of the pipe
    int pipe3_write = atoi(argv[0]);
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
        sleep(message->DelS1);
        if(strcmp(message->IdReceiver, "R3") != 0) {
            write_pipe(pipe3_write, message);
        }
    }
    */
    //free(buffer);
    close(fd_fifo);
    //close_pipe(pipe3_write);
    unlink("OutputFiles/my_fifo.txt");
    return 0;
}