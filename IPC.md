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

# II. Communication Facilities
## 1. Pipes
### 1.1 Definition
A pipe is a mechanism provided by the operating system to enable communication between processes. It acts as a unidirectional data channel, allowing one process to send data to another process.

![image](https://github.com/user-attachments/assets/772cc6d3-eb0f-4145-b04c-d53557accf32)

### 1.2 Features
- Unidirectional: Data flows in one direction (e.g., process A writes to the pipe, process B reads from it).
- Temporary Storage: Acts as a buffer between processes, storing data until it is read.
- Synchronous by Default:
  - A read operation blocks until data is available.
  - A write operation blocks if the pipe is full.
- Inter-related Processes: Generally used between processes with a shared ancestry (e.g., parent-child processes).
- Automatic Cleanup: The operating system handles the pipe's lifecycle.

### 1.3 Types of Pipes
- Unnamed Pipes:
  - Created using the `pipe()` system call.
  - Unidirectional: Requires two pipes for bidirectional communication.
  - Parent-Child Only: Works only between processes with a parent-child relationship.
  - No Name: Exists only in memory; cannot be accessed by unrelated processes.
- Named Pipes (FIFOs):
  - Created using the `mkfifo()` system call.
  - Bidirectional: Can handle communication in both directions, although this requires careful synchronization.
  - Accessible by Unrelated Processes: Any process with access to the file can use it.
  - Persistent: Exists as a file in the filesystem.

### 1.4 System Calls
```c
int pipe(int pipefds[2]);
```

- `pipefds[0]`: File descriptor for reading.
- `pipefds[1]`: File descriptor for writing.

### 1.5 Soure code
https://github.com/PhamDuong1311/Viettel_report/tree/main/IPC
## 2. POSIX Message Queue
### 2.1 Definition
POSIX message queues are a form of interprocess communication (IPC) mechanism present in Linux systems. They make it easy and effective for processes to send and receive messages. 

![image](https://github.com/user-attachments/assets/28f58e27-b0cf-40fd-9943-369f68d3eb69)

### 2.2 Features
- POSIX message queues store messages in queues that different processes can use. Each message in the queue has a priority, which makes it possible to get messages in a certain order. Processes can send messages to a queue and get messages from a queue. This lets different processes talk to each other and share info at the same time.
- One of the best things about POSIX message queues is that they allow for asynchronous contact. This means that processes can send and receive messages without waiting for each other. This makes them good for situations in which processes need to talk to each other without having to wait for each other to answer.
- POSIX message queues are given a unique name in the file system. This makes it possible for multiple processes to use the same queue by referring to its name. They can also be made as secret queues that only the process of making them can see.
=> Overall, POSIX message queues are a type of IPC mechanism present in Linux systems. Processes can send information to each other through named or unnamed queues. Each message in the queue has a priority, and messages can be read either in the order they were added to the queue (FIFO) or by priority.
### 2.3 System Calls
The POSIX message queues library provides several functions that allow processes to create, manage, send, and receive messages in message queues. Here are the key library functions used in POSIX message queues:
#### a. `mq_open()` function
This function is used to open an existing message queue or create a new one in POSIX message queues. It is part of the POSIX message queues library `<mqueue.h> and allows processes to access message queues for interprocess communication.

```c
mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
```

The arguments of `mq_open()` are:
- name: The name of the message queue. It should start with a forward slash (/) and be unique within the system. If the message queue doesn't exist, a new one will be created with the specified name.
- oflag: The open flags that control the behavior of `mq_open()`. Common flags include `O_RDONLY` (open for reading), `O_WRONLY` (open for writing), and `O_CREAT` (create the message queue if it doesn't exist).
- mode: The permissions to be set on the message queue if it is created. It is specified using the same format as file permissions (e.g., 0666).
- attr: A pointer to a `struct mq_attr` object that specifies the desired attributes of the message queue. If set to `NULL`, the default attributes will be used.

The return value of `mq_open()` is a message queue descriptor of type `mqd_t`. If the call is successful, the function returns a non-negative value representing the descriptor. On failure, it returns `-1`, and you can check the specific reason by examining the value of `errno`.
#### b. `mq_close()` function
This function is used to close a message queue in POSIX message queues. It is responsible for releasing any resources associated with the message queue and making the message queue descriptor available for reuse.

```c
int mq_close(mqd_t mqdes);
```

The argument `mqdes` is the message queue descriptor obtained when you create or open the message queue using functions like `mq_open()`. It represents the message queue that you want to close.

The return value of `mq_close()` is an integer indicating the success or failure of the operation. It returns 0 on success, indicating that the message queue was closed successfully. If an error occurs, it returns -1, and you can check the specific reason by examining the value of `errno`.
#### c. `mq_unlink()` function
This function is used to remove a message queue from the system in POSIX message queues. It allows you to unlink and delete a named message queue, freeing up system resources associated with it.

```c
int mq_unlink(const char *name);
```

The `name` argument is a string representing the name of the message queue to be unlinked. The name should start with a forward slash (/) and be unique within the system.

The return value of `mq_unlink()` is an integer indicating the success or failure of the operation. It returns 0 on success, indicating that the message queue was successfully unlinked and removed from the system. If an error occurs, it returns -1, and you can check the specific reason by examining the value of `errno`.
#### d. `mq_send()` function
This function is used to send a message to a message queue in POSIX message queues. It allows processes to send data and communicate with other processes through the message queue.

```c
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);
```

The arguments of `mq_send()` are:
- mqdes: The message queue descriptor obtained when you create or open the message queue using functions like `mq_open()`. It represents the message queue to which the message will be sent.
- msg_ptr: A pointer to the message data that you want to send. The message data should be a character array or a pointer to a structure.
- msg_len: The size of the message data in bytes.
- msg_prio: The priority of the message. Messages in the message queue can have different priorities, and the priority value is used to determine the order of message retrieval. Higher priority values indicate higher priority messages.

The return value of `mq_send()` is an integer indicating the success or failure of the operation. It returns 0 on success, indicating that the message was successfully sent to the message queue. If an error occurs, it returns -1, and you can check the specific reason by examining the value of `errno`.
#### e. `mq_receive()` function
This function is used to receive a message from a message queue in POSIX message queues. It allows processes to retrieve and read messages that have been sent to the message queue by other processes.
```c
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);
```

The arguments of `mq_receive()` are:
- mqdes: The message queue descriptor obtained when you create or open the message queue using functions like `mq_open()`. It represents the message queue from which the message will be received.
- msg_ptr: A pointer to a buffer where the received message will be stored. The received message will be copied into this buffer.
- msg_len: The size of the buffer in bytes, indicating the maximum length of the message that can be received.
- msg_prio: A pointer to an unsigned integer where the priority of the received message will be stored. Upon successful completion of `mq_receive()`, the priority of the received message will be stored in this variable.

The return value of `mq_receive()` is the number of bytes received and copied into the buffer. If an error occurs, it returns -1, and you can check the specific reason by examining the value of errno.
#### f. `mq_getattr()` function
This function is used to retrieve the attributes of a message queue in POSIX message queues. It allows you to obtain information about the message queue, such as its current size, maximum number of messages, and maximum message size.

```c
int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
```

The arguments of `mq_getattr()` are:
- mqdes: The message queue descriptor obtained when you create or open the message queue using functions like `mq_open()`. It represents the message queue from which you want to retrieve the attributes.
- attr: A pointer to a `struct mq_attr` object where the attributes of the message queue will be stored. The `struct mq_attr` contains the following members:
  - mq_flags: Flags associated with the message queue.
  - mq_maxmsg: The maximum number of messages that the message queue can hold.
  - mq_msgsize: The maximum size of each message in the message queue.
  - mq_curmsgs: The current number of messages in the message queue.

The return value of `mq_getattr()` is an integer indicating the success or failure of the operation. It returns 0 on success, indicating that the attributes of the message queue were successfully retrieved and stored in the provided struct mq_attr object. If an error occurs, it returns -1, and you can check the specific reason by examining the value of errno.
### 2.4 Source code 
https://github.com/PhamDuong1311/Viettel_report/tree/main/IPC
## 3. POSIX Shared Memory
### 3.1 Definition
Shared memory is one of the fastest IPC mechanisms because it allows multiple processes to access the same region of memory directly. This shared memory region is allocated by the kernel and can be mapped into the address spaces of all involved processes.

![image](https://github.com/user-attachments/assets/0cb1af9a-8d11-4e63-9aab-2544fd008c39)

### 3.2 Steps for using shared memory in IPC
- Creation/Allocation of Shared Memory
- Mapping Shared Memory
- Synchronization
- Signaling
- Detachment and Deletion
### 3.3 Why is shared memory the fastest IPC mechanisnm?
**- For Message Queues, Pipes, and FIFOs (Data is copied twice):**
  - **Sender's Address Space to Kernel Space**: When the sender makes a send call, the data is first copied into a buffer managed by the kernel.
  - **Kernel Space to Receiver's Address Space**: When the receiver makes a receive call, the kernel **copies** the data to the receiver's memory space.
=> This additional copying introduces significant overhead, especially for large messages or frequent data exchanges.

**- Shared Memory:**
- The kernel creates a memory segment and **maps** it into the address spaces of all participating processes.
- Data written to this memory segment by one process is immediately accessible to other processes **without any copying overhead**.
- Processes communicate by directly reading and writing to this shared memory region, avoiding intermediate kernel involvement.
### 3.4 System Call
#### a. `shm_open` function
`shm_open` is like the open system call for files. It opens a POSIX shared memory object and makes it available to the calling process via the returned file descriptor.

```c
int shm_open (const char *name, int oflag, mode_t mode);
```

The arguments of `shm_open`:
- name: A null-terminated string of up to NAME_MAX characters, starting with / and containing no other slashes.
- oflag: A bitmask combining one of the following:
  - `O_RDONLY`: Open the object for read-only access.
  - `O_RDWR`: Open the object for reading and writing.
  - `O_CREAT`: Create the object if it doesn’t exist.
  - `O_EXCL`: When used with `O_CREAT`, ensures the call fails if the object already exists (sets `errno` to `EEXIST`).
  - `O_TRUNC`: Truncates the object to zero bytes if it already exists.
- mode (Optional if `O_CREAT` is not used):
  - Specifies the permissions for the object if it’s created.
  - Similar to file permissions in Linux (0666 for read/write for owner, group, and others).

Return Value: On success: A file descriptor (fd) is returned, which represents the shared memory object. On failure: Returns -1 and sets errno to indicate the error.
#### b. `shm_unlink` function
```c
int shm_unlink (const char *name);
```

`shm_unlink` removes the previously created POSIX shared memory object. The name is the name of the shared memory object as described under `shm_open`, above.
#### c. `ftruncate` function
```c
int ftruncate (int fd, off_t length);
```

The `ftruncate` system call makes the object referred to by the file descriptor, `fd`, of size `length` bytes. When a POSIX shared memory is created, it is of size zero bytes. Using `ftruncate`, we can make the POSIX shared memory object of size length bytes. 

`ftruncate` returns zero on success. In case of error, ftruncate` returns -1 and errno is set to the cause of the error.
#### d. `mmap` function
```c
void *mmap (void *addr, size_t length, int prot, int flags,
            int fd, off_t offset);
```

The `mmap` arguments for POSIX Shared Memory:
- addr:
  - Specifies the address at which the shared memory is mapped.
  - Generally set to `NULL`, allowing the kernel to choose an appropriate address.
- length:
  - The size of the memory region to map.
  - For simplicity, this should match the size of the shared memory object set using `ftruncate`.
- prot (Protection Flags): Defines the access permissions for the mapped memory region:
  - `PROT_READ`: Allows read access.
  - `PROT_WRITE`: Allows write access.
  - Combined for shared memory: `PROT_READ` | `PROT_WRITE`.
- flags:
  - Specifies the behavior of the mapped memory.
  - For shared memory, use: `MAP_SHARED`: Changes made to the memory region are visible to all processes sharing the memory.
- fd: The file descriptor returned by `shm_open`.
- offset:
  - Specifies the starting point of the mapping within the shared memory object.
  - Typically set to `0` to map from the beginning of the shared memory object.
 
Return Value:
- On success: Returns a pointer to the memory-mapped region.
- On failure: Returns `MAP_FAILED` (defined as `(void *)-1`) and sets `errno` to indicate the error.
#### e. `munmap` function
```c
int munmap (void *addr, size_t length);
```

`munmap` unmapps the shared memory object at location pointed by `addr` and having size, `length`. On success, munmap returns 0. In case of error, munmap returns -1 and errno is set to the cause of the error.
### 3.5 Source code
https://github.com/PhamDuong1311/Viettel_report/tree/main/IPC
# III. Synchronization

# IV. Signal
