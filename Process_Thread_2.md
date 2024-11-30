# Process and Thread
# II. Basic Thread
## 1 Introduction
In the traditional Unix model, when a process needs something performed by another entity, it `fork()` a child process and lets the child perform the processing. But there are problems with `fork()`:
- fork is expensive. Memory is copied from the parent to the child, all descriptors are duplicated in the child, and so on.
- IPC is required to pass information between the parent and child after the `fork()` (learn in the next part).

=> Threads help with both problems. Threads are sometimes called lightweight processes since a thread is "lighter weight" than a process.

## 2 Basic Thread functions: Creation and Termination
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
