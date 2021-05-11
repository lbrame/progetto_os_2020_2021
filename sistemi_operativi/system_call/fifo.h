/// @file fifo.h
/// @brief Contiene la definizioni di variabili e
///         funzioni specifiche per la gestione delle FIFO.

#pragma once
int generate_fifo(char* string);
int open_fifo(char* string, int type);
void close_fifo(int fd);