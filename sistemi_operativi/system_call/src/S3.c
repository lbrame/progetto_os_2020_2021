/// @file S3.c
/// @brief Contiene l'implementazione del processo S3 chiamato da SenderManager.

#include "defines.h"
#include "unistd.h"
#include <stdio.h>

int main(int argc, char * argv[]) {
    printf("Yo sono S3, ma dopo exec!\n");
    char data[] = "Id;Message;Id_Sender;Id_Receiver;Time_arrival;Time_departure";
    write_file("OutputFiles/F3.csv", data);
    sleep(3);
    return 0;
}