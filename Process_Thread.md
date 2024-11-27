# I. Process and Thread
## 1. Introduction
In the traditional Unix model, when a process needs something performed by another entity, it `fork()` a child process and lets the child perform the processing. But there are problems with `fork()`:
- fork is expensive. Memory is copied from the parent to the child, all descriptors are duplicated in the child, and so on.
- IPC is required to pass information between the parent and child after the `fork()` (learn in the next part).

=> Threads help with both problems. Threads are sometimes called lightweight processes since a thread is "lighter weight" than a process.

## 2. Different between Process and Thread
Process and threads are the basic components in OS. Process is the program under execution whereas the thread is part of process. Threads of a process can be used when same process is required multiple times. A process can consists of multiple threads.
### 2.1 Process
Processes are basically the programs that are dispatched from the ready state and are scheduled in the CPU for execution. PCB ( Process Control Block ) holds the context of process. A process can create other processes which are known as Child Processes. The process takes more time to terminate, and it is isolated means it does not share the memory with any other process. The process can have the following states new, ready, running, waiting, terminated, and suspended.
### 2.2 Thread
### Comparision


# II. IPC

# III. Mutex and Semaphore
