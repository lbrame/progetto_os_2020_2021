/// @file shared_memory.h
/// @brief Contiene la definizioni di variabili e funzioni
///         specifiche per la gestione della MEMORIA CONDIVISA.

#include "err_exit.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

#pragma once

/**
 * Generate shared memory area.
 * @param size size of the shared memory area to initialize
 * @return id of the shared memory
 */
int generate_shmem(size_t size);

/**
 * Remove shared memory area.
 * @param shmid id of the shared memory area to remove
 */
void destroy_shmem(int shmid);

/**
 * Attach current process to shared memory area.
 * @param shmid id of existing shared memory area to attach to
 * @return pointer where shared memory is attached
 */
int* attach_smem(int shmid);

/**
 * Detach current process from shared memory area.
 * @param ptr Pointer previously used to attach process and shmem area
 */
void detach_shmem(int* ptr);
