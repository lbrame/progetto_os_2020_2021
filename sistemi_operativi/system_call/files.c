//
// Created by laura on 22/05/21.
//
#include "err_exit.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int my_open_wmode(const char *pathname, int flags, mode_t mode) {
    int fd = open(pathname, flags, mode);
    if (fd == -1)
        ErrExit("open");
    return fd;
}

int my_open(const char *pathname, int flags) {
    int fd = open(pathname, flags);
    if (fd == -1)
        ErrExit("open");
    return fd;
}

ssize_t my_write(int fd, const void *buf, size_t count) {
    ssize_t numWrite = write(fd, buf, count);
    if (numWrite == -1)
        ErrExit("write");
    return numWrite;
}

ssize_t my_read(int fd, void *buf, size_t count) {
    ssize_t numRead = read(fd, buf, count);
    if (numRead == -1)
        ErrExit("write");
    return numRead;
};