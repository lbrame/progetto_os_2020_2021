#include <sys/types.h>

int my_open(const char *pathname, int flags);
int my_open_wmode(const char *pathname, int flags, mode_t mode);
ssize_t my_write(int fd, const void *buf, size_t count);
ssize_t my_read(int fd, void *buf, size_t count);
