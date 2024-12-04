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
### 2. Process types
 There are different types of processes in a Linux system. These types include user processes, daemon 
processes, and kernel processes.
#### 2.1 User Processes
 Most processes in the system are user processes. A user process is one that is initiated by a regular user 
account and runs in user space. Unless it is run in a way that gives the process special permissions, an 
ordinary user process has no special access to the processor or to files on the system that don't belong to 
the user who launched the process
#### 2.2 Daemon Process
- A daemon process is an application that is designed to run in the background, typically managing some kind 
of ongoing service. A daemon process might listen for an incoming request for access to a service.
-  Although daemon processes are typically managed as services by the root user, daemon processes often 
run as non-root users by a user account that is dedicated to the service. By running daemons under different 
user accounts, a system is better protected in the event of an attack.
- Systems often start daemons at boot time and have them run continuously until the system is shut down. 
Daemons can also be started or stopped on demand, set to run at particular system run levels.
#### 2.3 Kernel Processes
- Kernel processes execute only in kernel space. They are similar to daemon processes. The primary 
difference is that kernel processes have full access to kernel data structures, which makes them more 
powerful than daemon processes that run in user space. 
- Kernel processes also are not as flexible as daemon processes. You can change the behavior of a daemon 
process by changing configuration files and reloading the service. Changing kernel processes, however, 
may require recompiling the kernel.
### 3. Process state
 When a process is created, the system assigns it a state. The state field of the process descriptor describes 
the current state of the process. 

![image](https://github.com/user-attachments/assets/205d9494-a64a-494f-be23-3cda16c1e4e4)

The following typical process states are possible on computer systems of all kinds. In most of these states, processes are "stored" on main memory:
#### 3.1 Created state
When a process is first created, it occupies the "created" or "new" state. In this state, the process awaits admission to the "ready" state. Admission will be approved or delayed by a long-term scheduler (admission scheduler).
#### 3.2 Waiting state
- A "ready" or "waiting" process has been loaded into main memory (RAM) and is awaiting execution on a CPU (to be context switched onto the CPU by the dispatcher (short-term scheduler)). There may be many "ready" processes at any one point of the system's execution.
- A ready queue (run queue) is used in computer scheduling. The CPU is only capable of handling one process at a time. Processes that are ready for the CPU are kept in a queue for "ready" processes. 
#### 3.3 Running state
A process moves into the running state when it is chosen for execution. The process's instructions are executed by one of the CPUs of the system. There is at most one running process per CPU. A process can run in either of the two modes, namely **kernel mode** or **user mode.**
- Kernel mode:
  + Processes in kernel mode can access both: kernel and user addresses.
  + Kernel mode allows unrestricted access to hardware including execution of privileged instructions.
  + Various instructions (such as I/O instructions and halt instructions) are privileged and can be executed only in kernel mode.
  + A system call from a user program leads to a switch to kernel mode.
- User mode:
  + Processes in user mode can access their own instructions and data but not kernel instructions and data (or those of other processes).
  + When the computer system is executing on behalf of a user application, the system is in user mode. However, when a user application requests a service from the operating system (via a system call), the system must transition from user to kernel mode to fulfill the request.
  + User mode avoids various catastrophic failures:
    + is an isolated virtual address space for each process in user mode.
    + User mode ensures isolated execution of each process so that it does not affect other processes as such.
    + No direct access to any hardware device is allowed.
#### 3.4 Blocked state
#### 3.5 Terminated state
#### 3.6 Additional process states

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

### 5. Process Creation
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

### 6. Process Termination
### 7. Monitoring Child Processes
### 8. Program Execution

### 9. more

