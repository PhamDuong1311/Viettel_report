# I. Process and Thread
## 1. Introduction
In the traditional Unix model, when a process needs something performed by another entity, it `fork()` a child process and lets the child perform the processing. But there are problems with `fork()`:
- fork is expensive. Memory is copied from the parent to the child, all descriptors are duplicated in the child, and so on.
- IPC is required to pass information between the parent and child after the `fork()` (learn in the next part).

=> Threads help with both problems. Threads are sometimes called lightweight processes since a thread is "lighter weight" than a process.

## 2. Basic Process
### 2.1 Proccess and Programs

### 2.2 Process ID and Parent Process ID
- A process is an instance of an executing program. A program is a file containing a range of information that describes how to construct a process at run time.
- One program may be used to construct many processes, or, put conversely, many processes may be running the same program.
- A process is an abstract entity defined by the kernel to execute a program by allocating system resources. It includes user-space memory containing program code and variables, along with kernel data structures that track its state. These data structures store information such as process IDs, virtual memory tables, open file descriptors, signal handling, resource usage, limits, and the current working directory.
### 2.3 Memory layout of a Process

### 2.4 Virtual memory management
### 2.5 The stack and stack frames
### 2.6 Command-Line arguments (argc, argv)


## 3. Basic Thread functions: Creation and Termination
### 3.1 `pthread_create()` Function
When a program is started by `exec`, a single thread is created, called the initial thread or main thread. Additional threads are created by `pthread_create`.

**Syntax**:

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg);
```

**Arguments**:
- pthread_t *thread: A pointer to a `pthread_t` variable that will receive the new thread's ID.
- const pthread_attr_t *attr: A pointer to a `pthread_attr_t` structure that defines thread attributes. Pass `NULL` for default attributes.
- void *(*start_routine) (void *): A pointer to the function that the thread will run. This function must take a `void *` argument and return a `void *`.
- void *arg: A pointer to the argument passed to the `start_routine` function.

**Return**:
- 0: On success.
- Non-zero value: On failure, indicating the error code.

### 3.2 `pthread_join()` Function

**Syntax**:
```c
int pthread_join(pthread_t thread, void **retval);
```

**Arguments**:
- pthread_t thread: The ID of the thread to wait for.
- void **retval: A pointer to a location where the exit status of the thread will be stored. Can be `NULL` if the status is not needed.

**Return**:
- 0: On success.
- Non-zero value: On failure, indicating the error code.
### 3.3 `pthread_self()` Function
**Syntax**:
```c
pthread_t pthread_self(void);
```

**Returns**: thread ID of calling thread

### 3.4 `pthread_detach()` Function
**Syntax**:
```c
int pthread_detach(pthread_t thread);
```

**Arguments**:
- pthread_t thread: The ID of the thread to detach.
- 
**Return**:
- 0: On success.
- Non-zero value: On failure, indicating the error code.

### 3.5 `pthread_exit()` Function
**Syntax**:
```c
void pthread_exit(void *retval);
```

**Arguments**:
- void *retval: A pointer to the value that the thread will return (it can be any type, typically cast to `void *`).

**Return**: There is no return value. This function terminates the calling thread and makes the return value available to other threads that join the thread.

## 4. Different between Process and Thread
Process and threads are the basic components in OS. Process is the program under execution whereas the thread is part of process. Threads of a process can be used when same process is required multiple times. A process can consists of multiple threads.
### 4.1 Process
Processes are basically the programs that are dispatched from the ready state and are scheduled in the CPU for execution. PCB ( Process Control Block ) holds the context of process. A process can create other processes which are known as Child Processes. The process takes more time to terminate, and it is isolated means it does not share the memory with any other process. The process can have the following states new, ready, running, waiting, terminated, and suspended.
#### What's PCB (Process Control Block)
While creating a process, the operating system performs several operations. To identify the processes, it assigns a process identification number (PID) to each process. As the operating system supports multi-programming, it needs to keep track of all the processes. For this task, the process control block (PCB) is used to track the process’s execution status. A Process Control Block (PCB) is a data structure used by the operating system to manage information about a process.

![image](https://github.com/user-attachments/assets/0f8935be-efaa-4f27-a438-e8b5adf11610)

The Process Control Block (PCB) is stored in a special part of memory that normal users can’t access. This is because it holds important information about the process. Some operating systems place the PCB at the start of the kernel stack for the process, as this is a safe and secure spot.
### 4.2 Thread
Thread is the segment of a process which means a process can have multiple threads and these multiple threads are contained within a process. A thread has three states: Running, Ready, and Blocked.
The thread takes less time to terminate as compared to the process but unlike the process, threads do not isolate.

![image](https://github.com/user-attachments/assets/ac0cd03f-f94c-4520-aef5-53c89223826d)

### 4.3 Different between Process and Thread

![image](https://github.com/user-attachments/assets/b89eef3c-072e-496d-861d-3a90edbe0a97)

# II. IPC

# III. Mutex and Semaphore
