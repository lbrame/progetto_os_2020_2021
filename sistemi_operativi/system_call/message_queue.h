/// @file message_queue.h
/// @brief Contiene la definizioni di variabili e funzioni
///         specifiche per la gestione della MESSAGE QUEUE.

#pragma once

int msgGet();
void msgSnd(int msqid, char* text);
void msgRcv(int msqid);
void delete_msgqueue(int msqid);
void msgqueue_set_upper_limit(int msqid, int upperlimit);
