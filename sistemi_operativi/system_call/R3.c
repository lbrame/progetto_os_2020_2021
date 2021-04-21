#include <stdlib.h>
#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    int pipe2_write = atoi(&argv[0][1]);
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F4.csv", data);
    sleep(3);
    return 0;
}