/// @file shared_memory.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione della MEMORIA CONDIVISA.

#include "err_exit.h"
#include "shared_memory.h"
#include "defines.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

/**
 * Generate shared memory area.
 * @param size size of the shared memory area to initialize
 * @return id of the shared memory
 */
int create_shmem(size_t size, char * buffer, char * creator) {
    key_t key = 01101101;
    int shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    char *creation_time_shmem = (char *) calloc(9, sizeof(char));
    creation_time_shmem = getTime(creation_time_shmem);

    if (strcmp(creator, "RM") == 0) {
        char * localbuffer;
        localbuffer = join("SH", itoa(key), ';');
        localbuffer = join(localbuffer, "-", ';');
        localbuffer = join(localbuffer, creation_time_shmem, ';');
        strcpy(buffer, localbuffer);
    }

    return shmid;
}

int get_shmem(size_t size) {
    key_t key = 01101101;
    int shmid = shmget(key, size, S_IRUSR | S_IWUSR);
    if (shmid == -1)
        ErrExit("shmget");
    return shmid;
}

/**
 * Remove shared memory area.
 * @param shmid id of the shared memory area to remove
 */
void destroy_shmem(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        ErrExit("shmctl: failed to remove shared memory");
}

int* attach_shmem(int shmid) {
    int *ptr = (int *) shmat(shmid, NULL, 0);
    if (*ptr == -1)
        ErrExit("shmat: failed to attach to shared memory");
    return ptr;
}

void detach_shmem(int* ptr) {
    if (shmdt(ptr) == -1)
        ErrExit("shmdt: failed to deatch shared memory");
}
