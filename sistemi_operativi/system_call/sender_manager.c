/// @file sender_manager.c
/// @brief Contiene l'implementazione del sender_manager.

#include "err_exit.h"
#include "defines.h"
#include "shared_memory.h"
#include "semaphore.h"
#include "fifo.h"
#include "pipe.h"

// Custom includes
#include "unistd.h"
#include <sys/wait.h>
#include <errno.h>
#include "stdio.h"
#include "stdlib.h"
#include <sys/stat.h>

/**
 * Struct that defines a child process
 * @param child_id ID that specifies in what order children are executed
 * @param pid
 */
typedef struct {
    char *sender_id;
    pid_t pid;
} child_struct;

/**
 * append a struct to the given array
 * @param info_children a list where to put the data of the chi
 * @param sender_id the id of the child
 * @param pid the pid of the process
 * @param i the index where to put the struct
 */
void add_child(child_struct *info_children, char sender_id[], pid_t pid, int i) {
    child_struct* child = malloc(sizeof(child_struct));
    child->sender_id = sender_id;
    child->pid = pid;
    info_children[i-1] = *child;
}

/**
 * wrapper for fork funcion, generates a process and the gives it some code to exute
 * @param info_children a list where to put the data of the child
 */
void generate_child(child_struct *info_children, char* inputFile) {
    static int i = 0;
    char *i_str = itoa(i + 1);

    char *sender_id = join("S", i_str, NULL);
    // Generate child process S*
    pid_t S_ = fork();
    if (S_ == -1)
        ErrExit("Fork");
    i++;
    if (S_ != 0) {
        add_child(info_children, sender_id, S_, i);
    } else {
        // figlio esegue
        char *execl_path = join("./", sender_id, NULL);
        int r;
        if (i == 1){
            r = execl(execl_path, sender_id, inputFile,(char *)NULL);
        } else {
            r = execl(execl_path, sender_id, (char *)NULL);
        }
        if (r == -1)
            perror("execl");
    }

    while ((S_ = wait(NULL)) != -1);

    if (errno != ECHILD) {
        char *err_string = join("Child ", sender_id, NULL);
        ErrExit(err_string);
    }

}

/**
 * join all messages preparing the text to be outputted to file
 * @param info_children a list containing the data of the children
 * @param counter the number of children of the process
 * @param starter the header of the file
 * @return the string to be outputted to file
 */
char *concatenate(child_struct *info_children, int counter, char *starter) {
    char *outputBuffer;
    char *old_outputBuffer;
    for (int row = 0; row < counter; row++) {
        for (int field_n = 0; field_n <= 2; field_n++) {
            switch (field_n) {
                case 0:
                    if (row == 0) {
                        outputBuffer = join(info_children[row].sender_id, "", NULL);
                    } else {
                        old_outputBuffer = outputBuffer;
                        outputBuffer = join(outputBuffer, info_children[row].sender_id, NULL);
                        free(old_outputBuffer);
                    }
                    break;
                case 1:
                    old_outputBuffer = outputBuffer;
                    char *pid_s = itoa(info_children[row].pid);
                    outputBuffer = join(outputBuffer, pid_s, ';');
                    free(old_outputBuffer);
                    break;
                case 2:
                    old_outputBuffer = outputBuffer;
                    outputBuffer = join(outputBuffer, "\n", NULL);
                    free(old_outputBuffer);
                    break;
                default:
                    ErrExit("Concatenate");
            }
        }
    }
    //Added the static string to outputBuffer
    outputBuffer = join(starter, outputBuffer, '\n');
    outputBuffer = join(outputBuffer, "\0", NULL);
    return outputBuffer;
}


int main(int argc, char *argv[]) {
    // Dynamic allocation of the memory
    child_struct *info_children = (child_struct *) malloc(sizeof(child_struct) * 3);
    if (info_children == NULL) {
        ErrExit("malloc");
    }

    struct stat sb;
    // If ./OutputFiles does not exist, create it
    if (stat("OutputFiles", &sb) != 0)
        mkdir("OutputFiles", S_IRWXU);

    // Generation of 3 children
    generate_child(info_children, argv[1]);
    generate_child(info_children, argv[1]);
    generate_child(info_children, argv[1]);

    int number_of_children = 3;
    char *starter = "SenderID;PID";
    char *outputBuffer = concatenate(info_children, number_of_children, starter);
    // outputBuffer written on F8.csv
    write_file("OutputFiles/F8.csv", outputBuffer);

    // Free up old buffers
    free(outputBuffer);
    free(info_children);
    return 0;
}
