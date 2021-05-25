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
void msgSnd(int msqid, Message_struct* message) {
    // Message structure
    struct mymsg {
        long mtype;
        Message_struct* struct_message;
    } m;

    // Define message type
    m.mtype = 1;

    // Copy input text string into message structure
    ssize_t structSize = sizeof(Message_struct);
    m.struct_message = (Message_struct*)malloc(sizeof (Message_struct));
    if(m.struct_message == NULL)
        ErrExit("malloc message_queue");

    memcpy(m.struct_message, message, structSize);

    // Get size of the text field
    size_t mSize = sizeof(Message_struct);

    // Send message to queue
    if (msgsnd(msqid, &m, mSize, 0) == -1)
        ErrExit("msgsnd failed");
    free(mSize);
    free(structSize);
}

void msgRcv(int msqid) {
    // Message structure
    struct mymsg {
        long mtype;
        char mtext[100];
    } m;

    // Get size of the text field
    size_t mSize = sizeof(struct mymsg) - sizeof(long);

    // Wait for message of type 1
    if (msgrcv(msqid, &m, mSize, 1, 0) == -1)
        ErrExit("smgrcv");
    printf("%s message queue\n", m.mtext);
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
