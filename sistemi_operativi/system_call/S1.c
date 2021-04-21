/// @file S1.c
/// @brief Contiene l'implementazione del processo S1 chiamato da SenderManager.

#include "unistd.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "err_exit.h"


/**
 *
 * @param info_children
 * @param counter
 * @param starter
 * @return
 */
//char *concatenate(Message_struct *info_children, int counter, char *starter) {
//    char *outputBuffer;
//    char *old_outputBuffer;
//    for (int row = 0; row < counter; row++) {
//        for (int field_n = 0; field_n <= 8; field_n++) {
//            switch (field_n) {
//                case 0:
//                    if (row == 0) {
//                        outputBuffer = join(info_children[row].Id, "", NULL);
//                    } else {
//                        old_outputBuffer = outputBuffer;
//                        outputBuffer = join(outputBuffer, info_children[row].Id, NULL);
//                        free(old_outputBuffer);
//                    }
//                    break;
//                case 1:
//                    old_outputBuffer = outputBuffer;
//                    outputBuffer = join(outputBuffer, info_children[row].Message, ';');
//                    free(old_outputBuffer);
//                    break;
//                case 2:
//                    old_outputBuffer = outputBuffer;
//                    outputBuffer = join(outputBuffer, info_children[row].IdSender, ';');
//                    free(old_outputBuffer);
//                    break;
//                case 3:
//                    old_outputBuffer = outputBuffer;
//                    outputBuffer = join(outputBuffer, info_children[row].IdReceiver, ';');
//                    free(old_outputBuffer);
//                    break;
//                case 4:
//                    old_outputBuffer = outputBuffer;
//                    outputBuffer = join(outputBuffer, info_children[row].DelS1, ';');
//                    free(old_outputBuffer);
//                    break;
//                case 5:
//                    old_outputBuffer = outputBuffer;
//                    outputBuffer = join(outputBuffer, info_children[row].DelS2, ';');
//                    free(old_outputBuffer);
//                    break;
//                case 6:
//                    old_outputBuffer = outputBuffer;
//                    outputBuffer = join(outputBuffer, info_children[row].DelS3, ';');
//                    free(old_outputBuffer);
//                    break;
//                case 7:
//                    old_outputBuffer = outputBuffer;
//                    outputBuffer = join(outputBuffer, info_children[row].Type, ';');
//                    free(old_outputBuffer);
//                    break;
//                case 8:
//                    old_outputBuffer = outputBuffer;
//                    outputBuffer = join(outputBuffer, "\n", NULL);
//                    free(old_outputBuffer);
//                    break;
//                default:
//                    ErrExit("Concatenate");
//            }
//        }
//    }
//    outputBuffer = join(starter, outputBuffer, '\n');
//    outputBuffer = join(outputBuffer, "\0", NULL);
//    return outputBuffer;
//}


int main(int argc, char * argv[]) {
    // Reading of F0
    char* rPath = argv[0];
    int pipe1_write = atoi(&argv[1][1]);
    int pipe1_read = atoi(&argv[1][0]);
    int fd = open(rPath, O_RDONLY);
    if (fd == -1)
        ErrExit("open");
    char* row = read_line(fd);
    while (row != NULL) {
        row = read_line(fd);
        Message_struct *message = parse_message(row);

        free(row);
    }
    close(fd);
    sleep(1);
    return 0;
}