#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    int* pipe2 = (int *) argv[1];
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F4.csv", data);
    unlink("OutputFiles/my_fifo.txt");
    return 0;
}