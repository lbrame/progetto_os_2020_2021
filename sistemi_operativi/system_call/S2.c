/// @file S2.c
/// @brief Contiene l'implementazione del processo S2 chiamato da SenderManager.

#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F2.csv", data);
    sleep(2);
    return 0;
}