# Process and Thread
# I. Basic Process
## 1 Overview of Process
### 1.1 Proccess and Programs
- A process is an instance of an executing program. A program is a file containing a range of information that describes how to construct a process at run time.
- One program may be used to construct many processes, or, put conversely, many processes may be running the same program.
- A process is an abstract entity defined by the kernel to execute a program by allocating system resources. It includes user-space memory containing program code and variables, along with kernel data structures that track its state. These data structures store information such as process IDs, virtual memory tables, open file descriptors, signal handling, resource usage, limits, and the current working directory.
### 1.2 Process ID and Parent Process ID
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

### 1.3 Memory layout of a Process
The memory allocated to each process is composed of a number of parts, usually
referred to as segments. These segments are as follows:
- `Text Segment`: Contains the program's machine instructions. It is read-only to prevent accidental modification and sharable among processes running the same program (The text segment is shared among processes by leveraging **virtual memory** and **memory mapping** techniques).
- `Initialized Data Segment`: Holds global and static variables explicitly initialized. Values are loaded from the executable file during program loading.
- `Uninitialized Data Segment (BSS)`: Stores global and static variables not explicitly initialized, automatically set to 0 by the system. Space for these variables is allocated at runtime, not in the executable file.
- `Stack`: Dynamically grows or shrinks, containing stack frames for each active function call. Each frame stores local variables, arguments, and return values.
- `Heap`: Used for dynamic memory allocation during runtime. The top end is called the program break.

![image](https://github.com/user-attachments/assets/f4b763b8-0bb9-4f87-ab65-f55a928d5caf)

### 1.4 Virtual memory management
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
## 2. Process types
 There are different types of processes in a Linux system. These types include user processes, daemon 
processes, and kernel processes.
### 2.1 User Processes
 Most processes in the system are user processes. A user process is one that is initiated by a regular user 
account and runs in user space. Unless it is run in a way that gives the process special permissions, an 
ordinary user process has no special access to the processor or to files on the system that don't belong to 
the user who launched the process
### 2.2 Daemon Process
- A daemon process is an application that is designed to run in the background, typically managing some kind 
of ongoing service. A daemon process might listen for an incoming request for access to a service.
-  Although daemon processes are typically managed as services by the root user, daemon processes often 
run as non-root users by a user account that is dedicated to the service. By running daemons under different 
user accounts, a system is better protected in the event of an attack.
- Systems often start daemons at boot time and have them run continuously until the system is shut down. 
Daemons can also be started or stopped on demand, set to run at particular system run levels.
### 2.3 Kernel Processes
- Kernel processes execute only in kernel space. They are similar to daemon processes. The primary 
difference is that kernel processes have full access to kernel data structures, which makes them more 
powerful than daemon processes that run in user space. 
- Kernel processes also are not as flexible as daemon processes. You can change the behavior of a daemon 
process by changing configuration files and reloading the service. Changing kernel processes, however, 
may require recompiling the kernel.
## 3. Process state
 When a process is created, the system assigns it a state. The state field of the process descriptor describes 
the current state of the process. 

![image](https://github.com/user-attachments/assets/419c51fd-4419-4766-94ae-d8d258cc97a5)


The following typical process states are possible on computer systems of all kinds. In most of these states, processes are "stored" on main memory:
### 3.1 Created state
When a process is first created, it occupies the "created" or "new" state. In this state, the process awaits admission to the "ready" state. Admission will be approved or delayed by a long-term scheduler (admission scheduler).
### 3.2 Waiting state
- A "ready" or "waiting" process has been loaded into main memory (RAM) and is awaiting execution on a CPU (to be context switched onto the CPU by the dispatcher (short-term scheduler)). There may be many "ready" processes at any one point of the system's execution.
- A ready queue (run queue) is used in computer scheduling. The CPU is only capable of handling one process at a time. Processes that are ready for the CPU are kept in a queue for "ready" processes. 
### 3.3 Running state
A process moves into the running state when it is chosen for execution. The process's instructions are executed by one of the CPUs of the system. There is at most one running process per CPU. A process can run in either of the two modes, namely **kernel mode** or **user mode.**
- Kernel mode: The process has access to the memory system and can execute privileges.
- User mode: The process has access only to the main memory and data, but no access to system resources or other processes.
### 3.4 Blocked state
Whenever the process requests access to I/O needs input from the user or needs access to a critical region(the lock for which is already acquired) it enters the blocked or waits state. The process continues to wait in the main memory and does not require CPU. Once the I/O operation is completed the process goes to the ready state.
### 3.5 Terminated state
A process may be terminated, either from the "running" state by completing its execution or by explicitly being killed. In either of these cases, the process moves to the "terminated" state. The underlying program is no longer executing, but the process remains in the process table as a zombie process until its parent process calls the `wait()` system call to read its exit status, at which point the process is removed from the process table, finally ending the process's lifetime. 
### 3.6 Additional process states
#### a. Swapped out and waiting
(Also called suspended and waiting.) In systems that support virtual memory, a process may be swapped out, that is, removed from main memory and placed on external storage by the scheduler. From here the process may be swapped back into the waiting state.

#### b. Swapped out and blocked
(Also called suspended and blocked.) Processes that are blocked may also be swapped out. In this event the process is both swapped out and blocked, and may be swapped back in again under the same circumstances as a swapped out and waiting process (although in this case, the process will move to the blocked state, and may still be waiting for a resource to become available).

### 3.7 How does a Process move from one state to others?
A process can move between different states in an operating system based on its execution status and resource availability. Here are some examples of how a process can move between different states:
- **New to Ready**: When a process is created, it is in a new state. It moves to the ready state when the operating system has allocated resources to it and it is ready to be executed.
- **Ready to Running**: When the CPU becomes available, the operating system selects a process from the ready queue depending on various scheduling algorithms and moves it to the running state.
- **Running to Blocked**: When a process needs to wait for an event to occur (I/O operation or system call), it moves to the blocked state. For example, if a process needs to wait for user input, it moves to the blocked state until the user provides the input.
- **Running to Ready**: When a running process is preempted by the operating system, it moves to the ready state. For example, if a higher-priority process becomes ready, the operating system may preempt the running process and move it to the ready state.
- **Blocked to Ready**: When the event a blocked process was waiting for occurs, the process moves to the ready state. For example, if a process was waiting for user input and the input is provided, it moves to the ready state.
- **Running to Terminated**: When a process completes its execution or is terminated by the operating system, it moves to the terminated state.

### 3.8 Long-term scheduler and Short-term scheduler

![image](https://github.com/user-attachments/assets/38d72da3-802a-4676-ab3e-db12da04b47e)

## 4. Basic Process functions: Creation, Termination, Monitoring Child and Execution.
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

## 5. Process Creation
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

**Memory Semantics of `fork()`**
When `fork()` is called in a process, it creates a new child process. Conceptually, this involves creating copies of the parent’s memory segments (**text, data, heap, and stack**) for the child. However, performing an actual copy of the entire memory space of the parent process would be wasteful, especially since the `fork()` is often followed by an immediate `exec()` call, which replaces the child’s memory with a new program. Modern operating systems, including Linux, avoid this wasteful copying using two key techniques:
- Sharing the **Text Segment** (Code) Between Parent and Child:
  + The kernel marks the text segment (code) as read-only.
  + Both the parent and child processes share the same physical memory for the **code segment**.
  + Instead of duplicating the **text segment**, the kernel sets up the child’s page-table entries to refer to the same physical memory pages used by the paren. => This allows both processes to share the code without duplication.

- Copy-On-Write (COW) for **Data, Heap, and Stack Segments**:
  + The kernel uses copy-on-write for the **data, heap, and stack segments** of the parent process.
  + Initially, the child’s page-table entries point to the same physical memory pages as the parent’s, and these pages are marked as read-only.
  + If either the parent or the child modifies one of these pages, the kernel traps the modification and creates a copy of the page for the faulting process (the parent or the child).
  + After the modification, the parent and child each have their own private copy of the page, and any further changes made by either process are not visible to the other.

![image](https://github.com/user-attachments/assets/370dd1c4-9d4b-440c-8024-3b04093f39bb)

## 6. Process Termination
A process can terminate either abnormally (due to a signal) or normally (using system calls like _exit() or exit()).
### 6.1 `_exit()` System Call
```c
void _exit(int status);
```

The `_exit()` system call is used for normal process termination. It is UNIX-specific and doesn't return.
The status argument defines the termination status of the process, which the parent can retrieve using the `wait()` system call.

**Status Interpretation:**
- The bottom 8 bits of the status value are available to the parent.
- By convention, 0 indicates successful termination, and any nonzero value indicates an error or abnormal termination.
- Value Ranges:
  - Status values between 0 and 255 are valid.
  - Values greater than 128 can cause confusion in shell scripts, as the shell encodes signal termination as 128 + signal number. This makes status values in this range potentially ambiguous.
`_exit()` never returns and terminates the process immediately.
### 6.2 `exit()` Library Function
```c
void exit(int status);
```

- The `exit()` function provides a higher-level interface and is part of the C Standard Library.
- Before calling `_exit()`, `exit()` performs several actions:
  - Calls exit handlers (functions registered with atexit() or on_exit()).
  - Flushes the stdio stream buffers to ensure any buffered data is written before the process terminates.
  - Then, it calls _exit() with the provided status code.
- Unlike _exit(), which is specific to UNIX, exit() is a standard function available in any C implementation.
### 7. Monitoring Child Processes
 In many application designs, a parent process needs to know when one of its child
 processes changes state—when the child terminates or is stopped by a signal. There are two techniques used to monitor child processes: the `wait()` system
 call (and its variants) and the use of the `SIGCHLD` signal.
#### 7.1 Waiting on a Child Process
The `wait()` system call waits for one of the children of the calling process to termi
nate and returns the termination status of that child in the buffer pointed to by `status`.

```c
  pid_t wait(int *status);
```

 The wait() system call does the following:
 - If no (previously unwaited-for) child of the calling process has yet terminated,
 the call blocks until one of the children terminates. If a child has already termi
nated by the time of the call, `wait()` returns immediately.
- If status is not `NULL`, information about how the child terminated is returned in
 the integer to which status points. We describe the information returned in status
 in Section 26.1.3.
- The kernel adds the process CPU times (Section 10.7) and resource usage statistics
 (Section 36.1) to running totals for all children of this parent process.
- As its function result, wait() returns the process ID of the child that has terminated.
### 8. Program Execution

### 9. IPC

