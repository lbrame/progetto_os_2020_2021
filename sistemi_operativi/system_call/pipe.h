/// @file pipe.h
/// @brief Contiene la definizioni di variabili e
///         funzioni specifiche per la gestione delle PIPE.

#pragma once

#include <stdio.h>
#include "defines.h"

void generate_pipe(int fd[]);
void close_pipe(int fd);
ssize_t read_pipe(int fd, Message_struct *content);
void write_pipe(int fd,  Message_struct *buffer);
