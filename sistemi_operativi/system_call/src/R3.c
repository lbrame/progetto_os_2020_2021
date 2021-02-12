#include "defines.h"
#include "unistd.h"
#include "stdio.h"

int main(int argc, char * argv[]) {
    char data[] = "Id;Message;Id_Sender;Id_Receiver;Time_arrival;Time_departure";
    write_file("OutputFiles/F4.csv", data);
    sleep(3);
    return 0;
}