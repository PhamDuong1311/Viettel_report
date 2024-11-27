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
#### What's PCB (Process Control Block)
While creating a process, the operating system performs several operations. To identify the processes, it assigns a process identification number (PID) to each process. As the operating system supports multi-programming, it needs to keep track of all the processes. For this task, the process control block (PCB) is used to track the process’s execution status. A Process Control Block (PCB) is a data structure used by the operating system to manage information about a process.

![image](https://github.com/user-attachments/assets/0f8935be-efaa-4f27-a438-e8b5adf11610)

The Process Control Block (PCB) is stored in a special part of memory that normal users can’t access. This is because it holds important information about the process. Some operating systems place the PCB at the start of the kernel stack for the process, as this is a safe and secure spot.
### 2.2 Thread
Thread is the segment of a process which means a process can have multiple threads and these multiple threads are contained within a process. A thread has three states: Running, Ready, and Blocked.
The thread takes less time to terminate as compared to the process but unlike the process, threads do not isolate.

![image](https://github.com/user-attachments/assets/ac0cd03f-f94c-4520-aef5-53c89223826d)

### Comparision

![image](https://github.com/user-attachments/assets/b89eef3c-072e-496d-861d-3a90edbe0a97)

# II. IPC

# III. Mutex and Semaphore
