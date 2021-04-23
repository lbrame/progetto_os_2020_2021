#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "unistd.h"
#include "err_exit.h"
#include "pipe.h"

int main(int argc, char * argv[]) {
    int pipe3_read = atoi(argv[0]);
    int pipe4_write = atoi(argv[1]);

    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_content = (Message_struct *) malloc(sizeof(Message_struct));
    if (content == NULL || last_content == NULL)
        ErrExit("malloc S2");

    ssize_t status;
    do { // Read until it returns 0 (EOF)
        memcpy(last_content, content, sizeof(Message_struct));
        status = read_pipe(pipe3_read, content);
        if(content->Id == last_content->Id)
            continue;
        sleep(content->DelS2);
        if(strcmp(content->IdReceiver, "R2") != 0) {
//            write_pipe(pipe4_write, content);
        }

    } while (status > 0);

    close_pipe(pipe3_read);
    close_pipe(pipe4_write);
    free(content);
    free(last_content);
    sleep(2);
    return 0;
}