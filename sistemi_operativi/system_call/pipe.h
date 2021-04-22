/// @file pipe.h
/// @brief Contiene la definizioni di variabili e
///         funzioni specifiche per la gestione delle PIPE.

#pragma once

#include <stdio.h>

void generate_pipe(int fd[]);
void close_pipe(int fd);
ssize_t read_pipe(int fd, char *content);
void write_pipe(int fd, char *buffer);