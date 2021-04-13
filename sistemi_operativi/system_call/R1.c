#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F6.csv", data);
    sleep(1);
    return 0;
}