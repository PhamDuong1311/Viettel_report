# Process and Thread
# II. Basic Thread
## 1. Introduction
In the traditional Unix model, when a process needs something performed by another entity, it `fork()` a child process and lets the child perform the processing. But there are problems with `fork()`:
- fork is expensive. Memory is copied from the parent to the child, all descriptors are duplicated in the child, and so on.
- IPC is required to pass information between the parent and child after the `fork()` (learn in the next part).

=> Threads help with both problems. Threads are sometimes called lightweight processes since a thread is "lighter weight" than a process.
### 1.1 Overview
Like processes, threads are a mechanism that permits an application to perform
 multiple tasks concurrently. A single process can contain multiple threads. All of these threads are independently executing the same
 program, and they all share the same global memory, including the initialized data,
 uninitialized data, and heap segments. 
 
 ![image](https://github.com/user-attachments/assets/25d747bb-6c63-4dd6-9389-21c6e1bfc7fb)

 The threads in a process can execute concurrently. On a multiprocessor system,
 multiple threads can execute parallel. If one thread is blocked on I/O, other
 threads are still eligible to execute. 
 ### 1.2 Different between threads and processes
Following are some of the major differences between the thread and the processes :
- Processes do not share their address space while threads executing under same process share the address space.
- From the above point its clear that processes execute independent of each other and the synchronization between processes is taken care by kernel only while on the other hand the thread synchronization has to be taken care by the process under which the threads are executing.
- Context switching between threads is fast as compared to context switching between processes.
- The interaction between two processes is achieved only through the standard inter process communication while threads executing under the same process can communicate easily as they share most of the resources like memory, text segment etc.
### 1.3 User threads vs Kernel threads
- **User Space Threads**:
  - Creation and Management: These threads are created, controlled, and destroyed by user-level thread libraries (such as pthread in POSIX systems). The kernel is unaware of the existence of these threads.
  - Multitasking Model: User space threads follow co-operative multitasking, meaning that a thread must explicitly yield the CPU to another thread. The kernel cannot preempt them.
  - Context Switching: Context switching between user space threads is fast, as it does not involve the kernel. The switching happens entirely in user space.
  - Blocking Behavior: If one thread in the process blocks (e.g., on I/O), the entire process will be blocked because the kernel only knows about the process as a single entity. It does not know about the individual threads inside the process.
  - Advantages:
    - Faster context switching.
    - Lower overhead since no kernel involvement is required for thread switching.
  - Disadvantages:
    - If one thread blocks, the entire process may be blocked.
    - Cannot take full advantage of multi-core processors because the kernel doesn't know about individual threads.
- **Kernel Space Threads**:
  - Creation and Management: Kernel space threads are created, controlled, and destroyed by the kernel. Each thread has its own unique representation in the kernel, and the kernel is fully aware of all the threads in a process.
  - Multitasking Model: These threads follow preemptive multitasking, meaning the kernel can preempt a running thread if a higher priority thread is ready to execute. The scheduler in the kernel decides when to switch between threads.
  - Context Switching: Context switching between kernel threads requires a context switch that involves the kernel, so it is typically slower than user space thread switching.
  - Blocking Behavior: If one kernel thread blocks (e.g., on I/O), other threads in the same process can continue to execute because the kernel manages each thread independently.
  - Advantages:
    - More robust as individual threads can be scheduled and managed independently by the kernel.
    - Better handling of blocking operations because other threads in the process can continue to run.
    - Can fully utilize multi-core processors since each thread is independently scheduled by the kernel.
  - Disadvantages:
    - Slower context switching due to kernel involvement.
    - Higher overhead for managing threads compared to user space threads.
## 2. Basic Thread functions: Creation, Termination, IDs, Joining, Deatching...
### 2.1 `pthread_create()` Function
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

### 2.2 `pthread_join()` Function

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
### 2.3 `pthread_self()` Function
**Syntax**:
```c
pthread_t pthread_self(void);
```

**Returns**: thread ID of calling thread

### 2.4 `pthread_detach()` Function
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

### 2.5 `pthread_exit()` Function
**Syntax**:
```c
void pthread_exit(void *retval);
```

**Arguments**:
- void *retval: A pointer to the value that the thread will return (it can be any type, typically cast to `void *`).

**Return**: There is no return value. This function terminates the calling thread and makes the return value available to other threads that join the thread.

## 3. Thread Synchronization
https://github.com/PhamDuong1311/Viettel_report/blob/main/IPC.md
