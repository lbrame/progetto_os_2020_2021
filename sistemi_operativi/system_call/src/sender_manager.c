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
 * DEBUG FUNCTION
 * TODO: REMOVE ON FINISHED PROJECT
 * prints the data contained into the parameter structure
 * @param c a structure containing child data
 * */
void print_child(child_struct* c) {
    printf("sender_id: %s\n", c->sender_id);
    printf("pid: %d\n", c->pid);
}


void add_child(child_struct *info_children, char sender_id[], pid_t pid, int i) {
    child_struct* child = malloc(sizeof(child_struct));
    child->sender_id = sender_id;
    child->pid = pid;
    info_children[i-1] = *child;
}

void generate_child(child_struct *info_children) {
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
        printf("iteratore %d\n", i);
        printf("Sono il figlio %s\n", sender_id);
        char *execl_path = join("./", sender_id, NULL);
        int r = execl(execl_path, sender_id, (char *)NULL);
        if (r == -1)
            perror("execl");
    }

    while ((S_ = wait(NULL)) != -1)
        printf("wait() returned child %s of PID %d\n\n", sender_id, S_);

    if (errno != ECHILD) {
        char *err_string = join("Child ", sender_id, NULL);
        ErrExit(err_string);
    }

}

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
                    char* pid_s = itoa(info_children[row].pid);
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

    outputBuffer = join(starter, outputBuffer, '\n');
    outputBuffer = join(outputBuffer, "\0", NULL);
    return outputBuffer;
}

int main(int argc, char *argv[]) {
    child_struct *info_children = (child_struct *) malloc(sizeof(child_struct) * 3);
    if (info_children == NULL) {
        ErrExit("malloc");
    }
    int number_of_children = 3;
    generate_child(info_children);
    generate_child(info_children);
    generate_child(info_children);

    char *starter = "Sender Id;PID";
    char *outputBuffer = concatenate(info_children, number_of_children, starter);
    // Create F8.csv
    write_file("OutputFiles/F8.csv", outputBuffer);
    // TODO: finish S1
    return 0;
}
