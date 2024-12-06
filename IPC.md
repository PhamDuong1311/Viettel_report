# I. Inter-process communication overview
## 1. A Taxonomy of IPC Facilities
The rich variety of UNIX communication and synchroniza
tion facilities, dividing them into three broad functional categories:
- **Communication**: These facilities are concerned with exchanging data between
 processes.
- **Synchronization**: These facilities are concerned with synchronizing the actions
 of processes or threads.
- **Signals**: Although signals are intended primarily for other purposes, they can
 be used as a synchronization technique in certain circumstances.

![image](https://github.com/user-attachments/assets/60939ea4-a2f3-4c7e-9c88-a0ecd970f73c)

## 2. Communication Facilities
We can break the communication facilities into two categories:
- **Data-transfer facilities**:  In order to communicate, one process writes data to
 the IPC facility, and another process reads the data. These facilities require two
 data transfers between user memory and kernel memory: one transfer from
 user memory to kernel memory during writing, and another transfer from
 kernel memory to user memory during reading.

![image](https://github.com/user-attachments/assets/05da2c65-6e47-4cdd-bab7-1cb220c1c170)

Figure: Exchanging data between two processes using a pipe

- **Shared memory**: Shared memory allows processes to exchange information by
 placing it in a region of memory that is shared between the processes.  A process can
 make data available to other processes by placing it in the shared memory
 region. Because communication doesn’t require system calls or data transfer
 between user memory and kernel memory, shared memory can provide very
 fast communication.
### 2.1 Data transfer
We can further break data-transfer facilities into the following subcategories:
- **Byte stream**: The data exchanged via **pipes**, **FIFOs**, and **stream sockets** is an
 undelimited byte stream. Each read operation may read an arbitrary number of
 bytes from the IPC facility, regardless of the size of blocks written by the writer.
 This model mirrors the traditional UNIX “file as a sequence of bytes” model.
- **Message**: The data exchanged via **System V message queues**, **POSIX message
 queues**, and **datagram sockets** takes the form of delimited messages. Each read
 operation reads a whole message, as written by the writer process. It is not possible to read part of a message, leaving the remainder on the IPC facility; nor is
 it possible to read multiple messages in a single read operation.
- **Pseudoterminals**: A pseudoterminal is a communication facility intended for use
 in specialized situations.

 A few general features distinguish data-transfer facilities from shared memory:
- **Destructive Reads**:
  - In data-transfer facilities (e.g., pipes, sockets), reads are destructive. When a process reads data, the data is consumed and removed from the facility.
  - Other processes cannot access the same data once it has been read.
- **Automatic Synchronization**:
  - Synchronization between the reader and writer is handled automatically.
  - If no data is available to be read, the reading process will block (pause execution) until data becomes available.
  - This blocking behavior ensures that data is not lost and the reader waits until new data arrives.
### 2.2 Shared memory
 Most modern UNIX systems provide three flavors of shared memory: **System V
 shared memory**, **POSIX shared memory**, and **memory mappings**.  
 Note the following general points about shared memory:
- **Fast Communication**: Processes can directly read from and write to the same memory region, making it the fastest form of inter-process communication (IPC).
- **Synchronization Requirement**: The speed advantage is offset by the need for synchronization to prevent data inconsistency.
One process should not access shared memory while another is updating it, which could lead to race conditions.Semaphores or other synchronization primitives (e.g., mutexes) are commonly used to ensure mutual exclusion and prevent concurrent modifications.
- **Nondestructive Data Access and Visibility of Data**: Data placed in shared memory is visible to all of the processes that share that
 memory. (This contrasts with the destructive read semantics described above
 for data-transfer facilities.)

## 3. Synchronization Facilities
Synchronization allows processes to avoid doing things such as
 simultaneously updating a shared memory region or the same part of a file. With
out synchronization, such simultaneous updates could cause an application to pro
duce incorrect results.

UNIX systems provide the following synchronization facilities:
- **Semaphores**: A semaphore is an integer maintained by the kernel, with its value never going below 0. A process can increment or decrement the semaphore value. If a process attempts to decrement it below 0, the kernel blocks the operation until the value becomes non-negative. Semaphores are typically used to control access to shared resources. Binary semaphores (values 0 or 1) are commonly used to represent exclusive access to a resource, while counting semaphores (values greater than 1) are used when there are multiple instances of a shared resource.
- **File Locks**:  File locks synchronize processes accessing the same file or its regions. There are two types of locks:
  - **Read (shared) locks**: Multiple processes can hold read locks on a file or its part simultaneously.
  - **Write (exclusive) locks**: Only one process can hold a write lock, and no other process can access the file or its region while the lock is held. Linux provides two systems for file locking:
    - `flock()`: A simple locking mechanism for locking entire files.
    - `fcntl()`: A more advanced method allowing record-level locking (locking specific regions of files).
- **Mutexes and Condition Variables**: These are commonly used with POSIX threads (pthreads) for synchronization between threads. Mutexes ensure that only one thread/process can access a critical section, while condition variables allow threads to wait for specific conditions before proceeding.
## 4. Comparing IPC facilities


