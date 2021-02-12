#include "defines.h"
#include "unistd.h"

int main(int argc, char * argv[]) {
    char data[] = "Id;Message;Id_Sender;Id_Receiver;Time_arrival;Time_departure";
    write_file("OutputFiles/F5.csv", data);
    sleep(2);
    return 0;
}