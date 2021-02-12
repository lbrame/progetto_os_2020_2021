/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include "defines.h"
#include "unistd.h"
#include "stdio.h"

int main(int argc, char * argv[]) {
    char data[] = "Id;Message;Id_Sender;Id_Receiver;Time_arrival;Time_departure";
    write_file("OutputFiles/F2.csv", data);
    sleep(2);
    return 0;
}