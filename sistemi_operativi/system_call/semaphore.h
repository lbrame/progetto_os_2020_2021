/// @file semaphore.h
/// @brief Contiene la definizioni di variabili e funzioni
///         specifiche per la gestione dei SEMAFORI.

#pragma once

#include <sys/sem.h>


int semGet(int sem_num);

int createSem(int sem_num);

void semOp(int semid, unsigned short sem_num, short sem_op);

void P(int semid, unsigned short sem_num);

void V(int semid, unsigned short sem_num);
