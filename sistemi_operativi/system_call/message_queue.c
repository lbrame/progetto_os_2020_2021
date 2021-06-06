//
// Created by luca on 5/21/21.
//

#include "message_queue.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "err_exit.h"
#include <stdio.h>
#include "defines.h"
#include <stdlib.h>
#define MAX 50


/**
 * Wrapper function to quickly create or get the message queue. The key is hard-coded
 * as per project specification.
 * @return IPC id of the newly created message queue
 */
int msgGet() {
    key_t key = 01110001;
    int msqid = msgget(key, IPC_CREAT | S_IRUSR | S_IWUSR);

    if (msqid == -1)
        ErrExit("msgget: create message queue");

    return msqid;
}

/**
 * Wrapper function for msgsnd()
 * @param msqid ID of the message queue to send message to
 * @param text Text to send as part of the message
 */
void msgSnd(int msqid, char* outputbuffer) {
    // Message structure
    struct mymsg {
        long mtype;
        char buffer[MAX];
    } m;

    // Define message type
    m.mtype = 1;
    strcpy(m.buffer, outputbuffer);
    int buf_length = strlen(m.buffer) + 1 ;
    if(msgsnd(msqid, &m, buf_length, IPC_NOWAIT) < 0)
        ErrExit("msgsnd failed");

}

char* msgRcv(int msqid, char* outputbuffer) {
    // Message structure
    struct mymsg {
        long mtype;
        char buffer[MAX+1];
    } m;

    m.mtype = 1;

    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) < 0)
        ErrExit("msgctl");

    if(buf.msg_qnum > 0) {
        //received the message from the queue
        if (msgrcv(msqid, &m, MAX, 1, 0) < 0)
            ErrExit("msgrcv");
        outputbuffer = (char*) malloc(sizeof (char)*(MAX));
        //decreasing the number of messages in queue
        return strcpy(outputbuffer, m.buffer);
    }
    else return NULL;
}

/**
 * Removes the desired message queue.
 * @param msqid Message queue to delete
 */
void delete_msgqueue(int msqid) {
    if (msgctl(msqid, IPC_RMID, NULL) == -1)
        ErrExit("msgctl failed: remove message queue");
}

/**
 * Change upper limit of a message queue.
 * @param msqid Message queue to change upper limit of
 * @param upperlimit New upper limit
 */
void msgqueue_set_upper_limit(int msqid, int upperlimit) {
    struct msqid_ds ds;

    if (msgctl(msqid, IPC_STAT, &ds) == -1)
        ErrExit("msgctl failed: get msqid_ds structure");

    ds.msg_qbytes = upperlimit;

    if (msgctl(msqid, IPC_SET, &ds) == -1)
        ErrExit("msgctl failed: update msqid_ds structure");
}

