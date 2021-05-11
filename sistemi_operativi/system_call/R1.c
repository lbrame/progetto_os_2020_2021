  #include "defines.h"
#include "unistd.h"
#include "pipe.h"
#include "err_exit.h"
#include <stdlib.h>
#include <memory.h>

int main(int argc, char * argv[]) {
    int pipe4_read = atoi(&argv[0][0]);

    Message_struct *content = (Message_struct *) malloc(sizeof(Message_struct));
    Message_struct *last_content = (Message_struct *) malloc(sizeof(Message_struct));
    if (content == NULL || last_content == NULL)
        ErrExit("malloc S2");
    ssize_t status;
    do { // Read until it returns 0 (EOF)
        memcpy(last_content, content, sizeof(Message_struct));
        status = read_pipe(pipe4_read, content);
        if(content->Id == last_content->Id) {
            continue;
        }
        sleep(content->DelS1);
    } while (status > 0);

    close_pipe(pipe4_read);
    free(content);
    free(last_content);
    sleep(1);
    return 0;
}