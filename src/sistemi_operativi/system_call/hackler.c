/// @file client.c
/// @brief Contiene l'implementazione del client.

#include <stdio.h>
#include <sys/stat.h>
#include "err_exit.h"
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>


int main(int argc, char * argv[]) {
    char * rPath = argv[1];

    // Get current directory
    printf("relative path: %s\n", rPath);

    // Get file fileSize
    struct stat st;
    int statStatus = stat(rPath, &st);
    if (statStatus == -1) {
        ErrExit("stat");
    }
    int fileSize = st.st_size;

    // allocate buffer to read file of size fileSize + 1(space for /0)
    char* inputBuffer = malloc(sizeof(char) * fileSize+1);
    if (inputBuffer == NULL) {
        ErrExit("malloc");
    }

    // read file
    int fd = open(rPath, O_RDONLY);
    if (fd == -1)
        ErrExit("open");
    ssize_t numRead = read(fd, inputBuffer, fileSize);
    if (numRead == -1) {
        ErrExit("read");
    }
    close(fd);

    return 0;
}