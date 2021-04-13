#include "defines.h"
#include "unistd.h"
#include <stdlib.h>

int main(int argc, char * argv[]) {
    int* fd1 = (int *) argv[0];
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F6.csv", data);
    sleep(1);
    return 0;
}