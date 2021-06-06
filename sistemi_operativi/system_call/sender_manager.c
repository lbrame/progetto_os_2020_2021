/// @file sender_manager.c
/// @brief Contiene l'implementazione del sender_manager.

#include "err_exit.h"
#include "defines.h"
#include "shared_memory.h"
#include "semaphore.h"
#include "fifo.h"
#include "pipe.h"
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// definition of the union semun
union semun {
    int val;
    struct semid_ds * buf;
    unsigned short * array;
} arg2;

/**
 * append a struct to the given array
 * @param info_children a list where to put the data of the chi
 * @param sender_id the id of the child
 * @param pid the pid of the process
 * @param i the index where to put the struct
 */
void add_child(child_struct *info_children, char sender_id[], pid_t pid, int i) {
    child_struct *child = (child_struct *) malloc(sizeof(child_struct));
    if (info_children == NULL) {
        ErrExit("malloc AddChild sender_manager");
    }
    child->sender_id = sender_id;
    child->pid = (int) pid;
    printf("S%d pid from SM: %d\n", i, pid);
    info_children[i - 1] = *child;
}

/**
 * wrapper for fork() function, generates a process and the gives it some code to execute
 * @param info_children a list where to put the data of the child
 */
void generate_child(child_struct *info_children, char *inputFile, const int fd1[2], const int fd2[2]) {
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
        // child process execute
        char *execl_path = join("./", sender_id, NULL);
        int r;
        switch (i) {
            case 1:
                close_pipe(fd1[0]);
                close_pipe(fd2[0]);
                close_pipe(fd2[1]);
                r = execl(execl_path, inputFile, itoa(fd1[1]), (char *) NULL);
                break;
            case 2:
                close_pipe(fd1[1]);
                close_pipe(fd2[0]);
                r = execl(execl_path, itoa(fd1[0]), itoa(fd2[1]), (char *) NULL);
                break;
            default:
                close_pipe(fd1[0]);
                close_pipe(fd1[1]);
                close_pipe(fd2[1]);
                r = execl(execl_path, itoa(fd2[0]), (char *) NULL);
                break;
        }
        if (r == -1)
            perror("execl");
    }

    if (errno == ECHILD) {
        char *err_string = join("Child ", sender_id, NULL);
        ErrExit(err_string);
    }

}

int main(int argc, char *argv[]) {

    struct stat sb;
    // Create semaphore set
    /* 0 -> shmem*/
    int semaphore_array = createSem(1);
    arg2.val = 0;

    // Shared memory
    int shmemId = create_shmem(sizeof(Message_struct));

    // Dynamic memory allocation
    child_struct *info_children = (child_struct *) malloc(sizeof(child_struct) * 3);
    if (info_children == NULL) {
        ErrExit("malloc senderManager");
    }

    // If ./OutputFiles does not exist, create it
    if (stat("OutputFiles", &sb) != 0)
        mkdir("OutputFiles", S_IRWXU);

    // if my_fifo.txt already exists, remove it
    struct stat sb2;
    if (stat("OutputFiles/my_fifo.txt", &sb2) == 0)
        remove("OutputFiles/my_fifo.txt");

    // create fifo
    generate_fifo("OutputFiles/my_fifo.txt");
    // close fifo
//    close_fifo(fifo);




    // create pipes
    int pipe1[2];
    int pipe2[2];
    generate_pipe(pipe1);
    generate_pipe(pipe2);

    // create child processes
    generate_child(info_children, argv[1], pipe1, pipe2);
    generate_child(info_children, argv[1], pipe1, pipe2);
    generate_child(info_children, argv[1], pipe1, pipe2);

    close_pipe(pipe1[0]);
    close_pipe(pipe1[1]);
    close_pipe(pipe2[0]);
    close_pipe(pipe2[1]);

    int number_of_children = 3;
    char *starter = "SenderID;PID";
    char *outputBuffer = manager_concatenate(info_children, number_of_children, starter);
    // outputBuffer written on F8.csv
    write_file("OutputFiles/F8.csv", outputBuffer);

    // wait for children
    while (wait(&info_children[0].pid) != -1);
    while (wait(&info_children[1].pid) != -1);
    while (wait(&info_children[2].pid) != -1);

    free(outputBuffer);
    free(info_children);

    return 0;
}
