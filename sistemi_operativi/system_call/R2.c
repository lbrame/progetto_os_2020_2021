#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    char data[] = "ID;Message;IDSender;IDReceiver;TimeArrival;TimeDeparture";
    write_file("OutputFiles/F5.csv", data);
    sleep(2);
    return 0;
}