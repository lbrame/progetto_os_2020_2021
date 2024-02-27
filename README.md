# Operating Systems Project

This is a project done for the Operating Systems course at Università di Verona. The official assignment and specification is located in the `assignment/` directory.

## Specification

The aim of this project is to create an application that simulates various types of sequential Inter-Process Communication (in a POSIX environment) where a "hackler" process is set to interfere with the intended communication between the various processes by modifying the specified communication sequence by signalling the various processes involved in the communcation.

When the program starts, a **SenderManager** (`SM`) project forks itself into 3 child Sender processes (`S1`, `S2`, `S3`) and connects `S1` to `S2` and `S3` to `S3` using either two UNIX PIPEs or FIFOs. The `SM` also generates a message queue, a shared memory region, a semaphore to synchronize exclusive access to the shared memory region, and a PIPE or FIFO. `S1`, `S2` and `S3` may send messages to the message queue and write to the shared memory area. On top of that, `S3` may also send data to Receiver 3 (`R3`)  using a FIFO. The parent `SM` process saves the mapping between each child process and its `PID` in `F8.csv`.

Then, the **ReceiverManager** (`RM`) process forks itslef into 3 Receiver children (`R1`, `R2`, `R3`) and, similarly to above, it connects `R3` to `R2` and `R2` to `R1` with, respectively, either two PIPEs (`PIPE3` and `PIPE4`) or two FIFOs (`FIFO3` and `FIFO4`). All three Receiver processes must be able to read from the message queue and from the shared memory. `R3` may also read from the PIPE or FIFO. Parent `RM` process saves the mapping between each child process and its PID to `F9.csv`.

Lastly, the third process that gets started when the program runs is the **Hackler** process, which may compromise the communication between the aforementioned processes by signalling the child Sender or Receiver processes.

As soon as `SM` is done with initializing everything it needs, `S1` reads a list of information formatted as such from the `F0.csv` file:

`ID | Message | IDSender | IDReceiver | DelS1 | DelS2 | DelS3 | Type`

where:

* `ID`: Unique message identifier. Chosen in chronological order.
* `Message`: Contents of the text in the message. 50 chars long at most.
* `IDSender`: PID of the sender process that will send the message (`S1`, `S2`, `S3`)
* `IDRecevier`: PID of the receiver process that will receive the message (`R1`, `R2`, `R3`)
* `DelSN`: Number of seconds the message must wait within a data structure associated to `SN`
* `Type`: POSIX Primitive used for communication (eg: `Q`, `SH`, `FIFO`)

Every line corresponds to a message that one specific Sender (`IDSender`) must send to one specific Receiver (`IDReceiver`) through a specific type of communication (`Type`).
The `S1` process generates a `struct` for every message that contains all information that is useful for the purpose of sending the message.
Lastly, `S1` starts a timer for every message that gets sent in order to delay the amount of seconds the message must be send after by `DelS1` seconds.

## Thanks

I wouldd like to thank the people whom I worked with during this project, and that have been amazing to work together with:

* Laura Canaia
* Lorenzo Bonanni
