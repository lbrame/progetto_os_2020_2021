/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include <stdlib.h>
#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    int pipe1_read = atoi(&argv[0][0]);
    int pipe2_write = atoi(&argv[1][1]);

    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F2.csv", data);
    sleep(2);
    return 0;
}