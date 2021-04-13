#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    int* fd1 = (int *) argv[0];
    int* fd2 = (int *) argv[1];
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F5.csv", data);
    sleep(2);
    return 0;
}