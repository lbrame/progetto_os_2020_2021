/// @file S1.c
/// @brief Contiene l'implementazione del processo S1 chiamato da SenderManager.

#include "unistd.h"
#include "malloc.h"
#include "defines.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
    printf("Hei sono arrivata qui!!!!!\n");
    char rPath[] = "InputFiles/F0.csv";
    int file_size = get_file_size(rPath);
    char* inputBuffer = (char *)malloc(file_size);
    read_file(inputBuffer, rPath,file_size);
    printf("Sto per stampare inputBuffer\n");
    printf("%s", inputBuffer);
    sleep(1);
    // TODO parse string
    // TODO add parsed string to struct
    // TODO write struct to file
    printf("S1, tutto finito\n");
    return 0;
}