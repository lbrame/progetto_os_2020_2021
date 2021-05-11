/// @file semaphore.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione dei SEMAFORI.

#include "err_exit.h"
#include "semaphore.h"

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
        ErrExit("semop failed in semOp wrapper");
}

/**
 * Signal
 * @param semid
 * @param sem_num
 */
void V(int semid, unsigned short sem_num) {
    struct sembuf sop = {.sem_num = sem_num, .sem_op = 1, .sem_flg = 0};

    if (semop(semid, &sop, 1) != -1)
        ErrExit("semop failed in semOp wrapper");
}