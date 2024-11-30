# I. Process and Thread
## 1. Basic Process
### 1.1 Overview of Process
#### a. Proccess and Programs
- A process is an instance of an executing program. A program is a file containing a range of information that describes how to construct a process at run time.
- One program may be used to construct many processes, or, put conversely, many processes may be running the same program.
- A process is an abstract entity defined by the kernel to execute a program by allocating system resources. It includes user-space memory containing program code and variables, along with kernel data structures that track its state. These data structures store information such as process IDs, virtual memory tables, open file descriptors, signal handling, resource usage, limits, and the current working directory.
#### b. Process ID and Parent Process ID
Each process has a process ID (PID), a positive integer that uniquely identifies the process on the system. The process ID is also useful if we need to build an identifier that is unique to a process.

The getpid() system call returns the process ID of the calling process.

```c
pid_t getpid(void);
```

The Linux kernel limits process IDs to being less than or equal to 32,767. When
a new process is created, it is assigned the next sequentially available process ID. Each
time the limit of 32,767 is reached, the kernel resets its process ID counter so that
process IDs are assigned starting from low integer values.

Each process has a parent—the process that created it. A process can find out the
process ID of its parent using the `getppid()` system call.

```c
pid_t getppid(void);
```

Each process has a parent process ID (PPID), reflecting a tree-like hierarchy of processes that traces back to `init` (PID 1), the ancestor of all processes. If a parent process terminates, orphaned child processes are adopted by init, and calls to `getppid()` return 1. 

#### c. Memory layout of a Process
The memory allocated to each process is composed of a number of parts, usually
referred to as segments. These segments are as follows:
- `Text Segment`: Contains the program's machine instructions. It is read-only to prevent accidental modification and sharable among processes running the same program (The text segment is shared among processes by leveraging **virtual memory** and **memory mapping** techniques).
- `Initialized Data Segment`: Holds global and static variables explicitly initialized. Values are loaded from the executable file during program loading.
- `Uninitialized Data Segment (BSS)`: Stores global and static variables not explicitly initialized, automatically set to 0 by the system. Space for these variables is allocated at runtime, not in the executable file.
- `Stack`: Dynamically grows or shrinks, containing stack frames for each active function call. Each frame stores local variables, arguments, and return values.
- `Heap`: Used for dynamic memory allocation during runtime. The top end is called the program break.

![image](https://github.com/user-attachments/assets/f4b763b8-0bb9-4f87-ab65-f55a928d5caf)

#### d. Virtual memory management
Linux employs a technique known as **virtual memory
management.** The aim of this technique is to make efficient use of both the **CPU** and
**RAM** (physical memory).

**Virtual memory** divides the memory space used by a program into small, fixed-size units called **pages**.
Similarly, the physical **RAM** is divided into **page frames** of the same size as the **pages**.

At any given time, only some pages of a program need to be loaded into physical memory (**RAM**). These pages form the **resident set**, which refers to the pages that are currently in memory and are actively being used by the process.

Copies of the **unused pages** of a program are maintained in the **swap
area** — a reserved area of disk space used to supplement the **computer’s RAM** — and
loaded into **RAM** only as required.

If a program tries to access a page that is not currently in the resident set (i.e., it’s not in physical memory), a **page fault** occurs. 
When a page fault happens, the kernel temporarily suspends the process, retrieves the **required page** from the **swap area or disk**, and loads it into the **physical memory (page frame)**.

![image](https://github.com/user-attachments/assets/ddf89f18-2a01-4595-ab4b-b9e3ba373d4e)

The kernel maintains a page table for each
process. The page table describes the location of each page in the process’s virtual address space (the set of all virtual memory pages available to the process).
Each entry in the page table either indicates the location of a virtual page in RAM
or indicates that it currently resides on disk.
### 1.2 Basic Process functions: Creation, Termination, Monitoring Child and Execution.
#### a. Overview of `fork()`, `exit()`, `wait()` and `execve()`
- `fork()` System Call:
  + Creates a new child process, which is an almost exact duplicate of the parent process.
  + The child inherits copies of the parent's stack, data, heap, and text segments.
- `exit(status)` Library Function:
  + Terminates a process and releases its resources (memory, open file descriptors, etc.).
  + The `status` argument specifies the process's termination status.
  + Layered on top of the `_exit()` system call, with `_exit()` being used when only one process should terminate after `fork()`.
- `wait(&status)` System Call:
  + Suspends the parent process until one of its child processes terminates.
  + Returns the termination status of the child through the `status` argument.
- `execve(pathname, argv, envp)` System Call:
  + Replaces the current process image with a new program specified by `pathname`.
  + `argv` and `envp` provide the argument and environment lists for the new program.
  + Discards the existing program's text, stack, data, and heap, creating fresh segments.
  + Often referred to as "execing" a new program, though there is no direct `exec()` call; instead, `execve()` and its variations are used.
 
![image](https://github.com/user-attachments/assets/9a845495-3e7c-4c7a-b554-6b7378e9ac7e)

#### b. Process Creation
- The `fork()` system call creates a new process, the child, which is an almost exact
duplicate of the calling process, the parent. The two processes are executing the same program **text**, but they have separate copies of the **stack**, **data**, and **heap** segments. The child’s stack, data, and heap segments are initially exact duplicates of the corresponding parts the parent’s memory. After the `fork()`, each process can modify the variables in its **stack**, **data**, and **heap** segments without affecting the other process.
- We can distinguish the two processes via the
value returned from `fork()`. For the parent, `fork()` returns the process ID of the
newly created child. For the child, `fork()` returns 0.
If necessary, the child can obtain its own process ID using `getpid()`, and the process
ID of its parent using `getppid()`.
- Example:

![image](https://github.com/user-attachments/assets/fcdb10de-8a28-4194-a3aa-3fd7703a6d4b)

=> Output:

![image](https://github.com/user-attachments/assets/c3149abe-a281-4430-a1a6-d429034ddabc)

#### c. Process Termination
#### d. Monitoring Child Processes
#### e. Program Execution
## 2. Basic Thread
### 2.1 Introduction
In the traditional Unix model, when a process needs something performed by another entity, it `fork()` a child process and lets the child perform the processing. But there are problems with `fork()`:
- fork is expensive. Memory is copied from the parent to the child, all descriptors are duplicated in the child, and so on.
- IPC is required to pass information between the parent and child after the `fork()` (learn in the next part).

=> Threads help with both problems. Threads are sometimes called lightweight processes since a thread is "lighter weight" than a process.

### 2.2 Basic Thread functions: Creation and Termination
#### a. `pthread_create()` Function
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

#### b. `pthread_join()` Function

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
#### c. `pthread_self()` Function
**Syntax**:
```c
pthread_t pthread_self(void);
```

**Returns**: thread ID of calling thread

#### d. `pthread_detach()` Function
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

#### e. `pthread_exit()` Function
**Syntax**:
```c
void pthread_exit(void *retval);
```

**Arguments**:
- void *retval: A pointer to the value that the thread will return (it can be any type, typically cast to `void *`).

**Return**: There is no return value. This function terminates the calling thread and makes the return value available to other threads that join the thread.

## 3. Different between Process and Thread
Process and threads are the basic components in OS. Process is the program under execution whereas the thread is part of process. Threads of a process can be used when same process is required multiple times. A process can consists of multiple threads.
### 3.1 Process
Processes are basically the programs that are dispatched from the ready state and are scheduled in the CPU for execution. PCB ( Process Control Block ) holds the context of process. A process can create other processes which are known as Child Processes. The process takes more time to terminate, and it is isolated means it does not share the memory with any other process. The process can have the following states new, ready, running, waiting, terminated, and suspended.
#### What's PCB (Process Control Block)
While creating a process, the operating system performs several operations. To identify the processes, it assigns a process identification number (PID) to each process. As the operating system supports multi-programming, it needs to keep track of all the processes. For this task, the process control block (PCB) is used to track the process’s execution status. A Process Control Block (PCB) is a data structure used by the operating system to manage information about a process.

![image](https://github.com/user-attachments/assets/0f8935be-efaa-4f27-a438-e8b5adf11610)

The Process Control Block (PCB) is stored in a special part of memory that normal users can’t access. This is because it holds important information about the process. Some operating systems place the PCB at the start of the kernel stack for the process, as this is a safe and secure spot.
### 3.2 Thread
Thread is the segment of a process which means a process can have multiple threads and these multiple threads are contained within a process. A thread has three states: Running, Ready, and Blocked.
The thread takes less time to terminate as compared to the process but unlike the process, threads do not isolate.

![image](https://github.com/user-attachments/assets/ac0cd03f-f94c-4520-aef5-53c89223826d)

### 3.3 Different between Process and Thread

![image](https://github.com/user-attachments/assets/b89eef3c-072e-496d-861d-3a90edbe0a97)

# II. IPC

# III. Mutex and Semaphore
