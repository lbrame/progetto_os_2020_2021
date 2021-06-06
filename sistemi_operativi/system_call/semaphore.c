/// @file semaphore.c
/// @brief Contiene l'implementazione delle funzioni
///         specifiche per la gestione dei SEMAFORI.

#include "err_exit.h"
#include "semaphore.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdio.h>

// definition of the union semun
union semun {
    int val;
    struct semid_ds * buf;
    unsigned short * array;
} arg;

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
    if (semid != -1) {
        // Initialize semaphore set to 1
        for (int i = 0; i < sem_num; i++) {
            arg.val = 1;
            if (semctl(semid, i, SETVAL, arg) == -1)
                ErrExit("semctl SETVAL in semGet wrapper");
        }
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
    printf("Oppure arrivo in semop?\n");
    struct sembuf sop = {.sem_num = sem_num, .sem_op = sem_op, .sem_flg = 0};

    if (semop(semid, &sop, 1) == -1)
        ErrExit("semop failed in semOp wrapper");
}

/**
 * Wait
 * @param semid
 * @param sem_num
 */
void P(int semid, unsigned short sem_num) {
    struct sembuf sop = {.sem_num = sem_num, .sem_op = -1, .sem_flg = 0};
    if (semop(semid, &sop, 1) == -1)
        ErrExit("semop failed in P wrapper");
}

/**
 * Signal
 * @param semid
 * @param sem_num
 */
void V(int semid, unsigned short sem_num) {
    struct sembuf sop = {.sem_num = sem_num, .sem_op = 1, .sem_flg = 0};

    // Make semaphore binary
    int value = semctl(semid, sem_num, GETVAL, 0/*ignored*/);
    if (value == 1)
        return;
    else if (value == -1)
        ErrExit("semctl GETVAL (V)");

    if (semop(semid, &sop, 1) == -1)
        ErrExit("semop failed in V wrapper");
}

void delete_sem(int semid){
    if (semctl(semid, 0/*ignored*/, IPC_RMID, 0/*ignored*/) == -1)
        ErrExit("semctl IPC_RMID");
}

void printSem(int sem_num, int semid) {
    // Print semaphore values
    for(int i=0; i<sem_num+1; i++) {
        int value = semctl(semid, i, GETVAL, 0/*ignored*/);
        if (value == -1)
            ErrExit("semctl GETVAL");
        printf("Semaphore at index %d has value %d\n", i, value);
    }
}