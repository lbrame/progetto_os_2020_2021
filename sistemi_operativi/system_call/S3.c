/// @file S3.c
/// @brief Contiene l'implementazione del processo S3 chiamato da SenderManager.

#include "defines.h"
#include "unistd.h"
#include <stdio.h>

int main(int argc, char * argv[]) {
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F3.csv", data);
    sleep(3);
    return 0;
}