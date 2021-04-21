/// @file pipe.h
/// @brief Contiene la definizioni di variabili e
///         funzioni specifiche per la gestione delle PIPE.

#pragma once
void generate_pipe(int fd[]);
void close_pipe(int fd);
char* read_pipe(int fd);
void write_pipe(int fd, char *buffer);