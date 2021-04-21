#include <stdlib.h>
#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    int pipe1_read = atoi(&argv[0][0]);
    int pipe2_write = atoi(&argv[1][1]);
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F5.csv", data);
    sleep(2);
    return 0;
}