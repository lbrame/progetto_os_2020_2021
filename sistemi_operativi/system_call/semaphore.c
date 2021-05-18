/// @file semaphore.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione dei SEMAFORI.

#include "err_exit.h"
#include "semaphore.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Simply call semget() on selected semaphore.
 *
 * @param key
 * @param sem_num number of semaphore to call semget() to
 * @return id of created semaphore
 */
int semGet(int sem_num) {
    key_t key = 01110011;

    int semid = semget(key, sem_num, S_IRUSR | S_IWUSR);
    if (semid == -1)
        ErrExit("SemGet ");

    return semid;
}


/**
 * Create sem with IPC_EXCL so that it only creates a semaphore if none exists.
 * Call this function in sender_manager and receiver_manager
 * @param sem_num
 * @return id of created semaphore
 */
int createSem(int sem_num) {
    key_t key = 01110011;

    int semid = semget(key, sem_num, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (semid == -1)
        ErrExit("SemGet ");

    // Initialize semaphore set to 1
    for(int i=0; i<sem_num; i++) {
        arg.val = 1;
        if (semctl(semid, i, SETVAL, arg) == -1)
            ErrExit("semctl SETVAL in semGet wrapper");
    }

    return semid;
}

/**
 * Wrapper function to run semop() on a specific semaphore more easily.
 * @param semid - id of the semaphore array
 * @param sem_num - semaphore number within the semid array
 * @param sem_op - operation to run on the selected semaphore
 */
void semOp(int semid, unsigned short sem_num, short sem_op) {
    struct sembuf sop = {.sem_num = sem_num, .sem_op = sem_op, .sem_flg = 0};

    if (semop(semid, &sop, 1) != -1)
        ErrExit("semop failed in semOp wrapper");
}

/**
 * Wait
 * @param semid
 * @param sem_num
 */
void P(int semid, unsigned short sem_num) {
    struct sembuf sop = {.sem_num = sem_num, .sem_op = -1, .sem_flg = 0};

    if (semop(semid, &sop, 1) != -1)
        ErrExit("semop failed in P wrapper");
}

/**
 * Signal
 * @param semid
 * @param sem_num
 */
void V(int semid, unsigned short sem_num) {
    struct sembuf sop = {.sem_num = sem_num, .sem_op = 1, .sem_flg = 0};

    if (semop(semid, &sop, 1) != -1)
        ErrExit("semop failed in V wrapper");
}