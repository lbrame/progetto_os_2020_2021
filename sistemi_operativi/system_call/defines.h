/// @file defines.h
/// @brief Contiene la definizioni di variabili
///         e funzioni specifiche del progetto.

#pragma once
typedef struct {
    char* Id;
    char* Message;
    char* IdSender;
    char* IdReceiver;
    int DelS1;
    int DelS2;
    int DelS3;
    char* Type;
} Message_struct;

int count_messages(const char *input, int fileSize);
int get_file_size(char *rPath);
void read_file(char *inputBuffer, char *rPath, int fileSize);
void write_file(char in_file_path[], char* outputBuffer);
char* get_out_file_rpath(char *in_file_path);
char* join (char* str1, char* str2, char join_character);
char* itoa(int val);
int read_line(int fd, char* buffer);
Message_struct *parse_message(char *inputBuffer);
long get_file_size_from_fd(int fd);