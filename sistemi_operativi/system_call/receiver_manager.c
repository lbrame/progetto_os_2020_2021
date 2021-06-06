/// @file receiver_manager.c
/// @brief Contiene l'implementazione del receiver_manager.

#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "defines.h"
#include "err_exit.h"
#include "pipe.h"
#include "semaphore.h"
#include "message_queue.h"
#include "shared_memory.h"
#include <fifo.h>
#include "files.h"
#include <string.h>


/**
 * append a struct to the given array
 * @param info_children a list where to put the data of the chi
 * @param sender_id the id of the child
 * @param pid the pid of the process
 * @param i the index where to put the struct
 */
void add_child(child_struct *info_children, char sender_id[], pid_t pid, int i) {
    child_struct* child = (child_struct*) malloc(sizeof(child_struct));
    if(child == NULL)
        ErrExit("malloc AddChild receiver_manager");

    child->sender_id = sender_id;
    child->pid = pid;
    info_children[i-1] = *child;
}

/**
 * wrapper for fork() function, generates a process and the gives it some code to execute
 * @param info_children a list where to put the data of the child
 */
void generate_child(child_struct *info_children, const int fd3[2], const int fd4[2]) {
    // the execution order should be R3, R2, R1 since R3
    // should receive messages from fifo
    static int i = 4;
    char *i_str = itoa(i-1);

    char *sender_id = join("R", i_str, NULL);
    // Generate child process S*
    pid_t S_ = fork();
    if (S_ == -1)
        ErrExit("Fork");
    i--;
    if (S_ != 0) {
        add_child(info_children, sender_id, S_, i);
    } else {
        // child process execute
        char *execl_path = join("./", sender_id, NULL);
        int r;
        switch (i) {
            case 1:
                close_pipe(fd3[0]);
                close_pipe(fd3[1]);
                close_pipe(fd4[1]);
                r = execl(execl_path, itoa(fd4[0]), (char *)NULL);
                break;
            case 2:
                close_pipe(fd3[1]);
                close_pipe(fd4[0]);
                r = execl(execl_path, itoa(fd3[0]), itoa(fd4[1]), (char *)NULL);
                break;
            default:
                close_pipe(fd3[0]);
                close_pipe(fd4[0]);
                close_pipe(fd4[1]);
                r = execl(execl_path, itoa(fd3[1]), (char *)NULL);
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



int main(int argc, char * argv[]) {
    // Message queue
    char * buffer_queue = malloc(50);
    int rm_queue = msgGet();
    char *creation_time_q = (char *) calloc(9, sizeof(char));
    creation_time_q = getTime(creation_time_q);
    buffer_queue = join("Q", itoa(01110001), ';');
    buffer_queue = join(buffer_queue, "-", ';');
    buffer_queue = join(buffer_queue, creation_time_q, ';');


    // Semaphore set
    // 0: protect shmem writes
    // 1: protect F10 writes
    // char * buffer_sem = malloc(31);
    char * buffer_sem = malloc(32);
    int semaphore_array = createSem(3, buffer_sem, "RM");
    if (semaphore_array == -1) {
        semaphore_array = semGet(3);
    }

    // Shared memory
    char * buffer = malloc(30);
    int shmemId = create_shmem(sizeof(Message_struct), buffer, "RM");
    if(shmemId == -1) {
        shmemId = get_shmem(sizeof(Message_struct));
    }



    // Dynamic memory allocation
    child_struct *info_children = (child_struct *) malloc(sizeof(child_struct) * 3);
    if (info_children == NULL) {
        ErrExit("malloc");
    }

    struct stat sb;
    // If ./OutputFiles does not exist, create it
    if (stat("OutputFiles", &sb) != 0)
        mkdir("OutputFiles", S_IRWXU);

    // create pipes
    int pipe3[2];
    int pipe4[2];
    generate_pipe(pipe3);
    generate_pipe(pipe4);

     generate_fifo("OutputFiles/custom_fifo1.txt");
     generate_fifo("OutputFiles/custom_fifo2.txt");
     // NON bisogna chiudere le fifo
     // le fifo sono solo un file!

    // create child processes
    generate_child(info_children, pipe3, pipe4);
    generate_child(info_children, pipe3, pipe4);
    generate_child(info_children, pipe3, pipe4);

    close_pipe(pipe3[0]);
    close_pipe(pipe3[1]);
    close_pipe(pipe4[0]);
    close_pipe(pipe4[1]);

    int number_of_children = 3;
    char *starter = "ReceiverID;PID";
    char *outputBuffer = manager_concatenate(info_children, number_of_children, starter);
    // outputBuffer written on F9.csv
    write_file("OutputFiles/F9.csv", outputBuffer);

    // wait for children
    while (wait(&info_children[0].pid) != -1);
    while (wait(&info_children[1].pid) != -1);
    while (wait(&info_children[2].pid) != -1);

    // Log SHMEM to F10
    printf("Buffer: %s\n", buffer);

    char *destruction_time_shmem = (char *) calloc(9, sizeof(char));
    destruction_time_shmem = getTime(destruction_time_shmem);
    buffer = join(buffer, destruction_time_shmem, ';');
    buffer = join(buffer, "\n", NULL);
    int fd_f10_rm = my_open("OutputFiles/F10.csv", O_WRONLY | O_APPEND);
    P(semaphore_array, 1);
    my_write(fd_f10_rm, buffer, strlen(buffer));
    V(semaphore_array, 1);
    close(fd_f10_rm);

    // Log SEM to F10
    printf("Buffer SEM: %s\n", buffer_sem);

    char *destruction_time_sem = (char *) calloc(9, sizeof(char));
    destruction_time_sem = getTime(destruction_time_sem);
    buffer_sem = join(buffer_sem, destruction_time_sem, ';');
    buffer_sem = join(buffer_sem, "\n", NULL);
    int fd_f10_sem = my_open("OutputFiles/F10.csv", O_WRONLY | O_APPEND);
    P(semaphore_array, 1);
    my_write(fd_f10_sem, buffer_sem, strlen(buffer_sem));
    V(semaphore_array, 1);
    close(fd_f10_sem);


    // Free up old buffers
    free(outputBuffer);
    free(info_children);
    unlink("OutputFiles/my_fifo.txt");
    unlink("OutputFiles/custom_fifo1.txt");
    unlink("OutputFiles/custom_fifo2.txt");
    delete_msgqueue(rm_queue);

    char *destruction_time_myfifo = (char *) malloc(sizeof(char) * 8);
    destruction_time_myfifo = getTime(destruction_time_myfifo);

    char *destruction_time_custom_fifo1 = (char *) malloc(sizeof(char) * 8);
    destruction_time_custom_fifo1 = getTime(destruction_time_custom_fifo1);

    char *destruction_time_custom_fifo2 = (char *) malloc(sizeof(char) * 8);
    destruction_time_custom_fifo2 = getTime(destruction_time_custom_fifo2);

    // Log Q to F10
    char *destruction_time_q = (char *) calloc(9, sizeof(char));
    destruction_time_q = getTime(destruction_time_q);
    buffer_queue = join(buffer_queue, destruction_time_q, ';');
    buffer_queue = join(buffer_queue, "\n", NULL);
    int fd_f10_q = my_open("OutputFiles/F10.csv", O_WRONLY | O_APPEND);
    P(semaphore_array, 1);
    my_write(fd_f10_q, buffer_queue, strlen(buffer_queue));
    V(semaphore_array, 1);
    close(fd_f10_q);

    // FIFO
    int tmp_size = get_file_size("OutputFiles/tmp.txt");
    char *tmp_read_buf = malloc(tmp_size);
    read_file(tmp_read_buf, "OutputFiles/tmp.txt", tmp_size);
    printf("tmp_read_buf: %s\n", tmp_read_buf);


    // Close semaphore array
    P(semaphore_array, 2);
    delete_sem(semaphore_array);
    destroy_shmem(shmemId);

    free(destruction_time_q);
    free(destruction_time_sem);
    free(destruction_time_shmem);
    free(destruction_time_myfifo);
    free(destruction_time_custom_fifo1);
    free(destruction_time_custom_fifo2);

    return 0;
}
