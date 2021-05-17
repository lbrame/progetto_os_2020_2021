/// @file shared_memory.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione della MEMORIA CONDIVISA.

#include "err_exit.h"
#include "shared_memory.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

int generate_shmem(size_t size) {
    int shmid;
    shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (shmid == -1)
        ErrExit("shmget");
    return shmid;
}

void destroy_shmem(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        ErrExit("shmctl: failed to remove shared memory");
}


int* attach_smem(int* shmid) {
    int *ptr = (int *) shmat(shmid, NULL, 0);
    if (*ptr == -1)
        ErrExit("shmat: failed to attach to shared memory");
    return ptr;
}

void detach_shmem(int* ptr) {
    if (shmdt(ptr) == -1)
        ErrExit("shmdt: failed to deatch shared memory");
}

