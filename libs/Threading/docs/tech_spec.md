# Thread Pool Module: Technical Documentation

This document outlines the design, structure, and usage of the `thread_pool.c` and `thread_pool.h` files. The thread pool is a portable, lightweight abstraction for submitting tasks to a fixed pool of worker threads using a blocking queue model.

---

## Overview

The thread pool provides:

- A fixed-size pool of worker threads
- A shared, thread-safe task queue (backed by the `queue` module)
- Graceful shutdown with `pthread_join`
- Optional cleanup of dynamically allocated task arguments
- Blocking semantics using condition variables in the queue

---

## Header File: `thread_pool.h`

### Constants

```c
#define MIN_THREADS ((size_t)2)
```

This constant defines the minimum number of threads required to instantiate a thread pool.

### Type Definitions

#### `task_function_t`

A pointer to the function executed by a worker thread.

```c
typedef void * (*task_function_t)(void * arg);
```

#### `arg_free_t`

An optional cleanup function for a task's argument.

```c
typedef void (*arg_free_t)(void * arg);
```

#### `thread_pool_t`

An opaque struct representing a thread pool. Its internal structure is hidden from users.

---

## API Functions

### Initialization and Destruction

```c
thread_pool_t * thread_pool_create(size_t thread_count);
int thread_pool_shutdown(thread_pool_t * thread_pool);
int thread_pool_destroy(thread_pool_t ** thread_pool);
```

- `thread_pool_create()` allocates and launches a pool of worker threads.
- `thread_pool_shutdown()` waits for all threads to finish.
- `thread_pool_destroy()` cleans up all memory and internal state.

### Task Submission

```c
int threadpool_add_task(thread_pool_t * thread_pool,
                        task_function_t task,
                        arg_free_t      arg_free,
                        void *          arg);
```

- Adds a task to the task queue for asynchronous execution.
- `task` must not be `NULL`.
- `arg` can be `NULL`.
- `arg_free` is optional; it will be called after the task completes if provided.

---

## Source File: `thread_pool.c`

### Internal Structures

#### `task_t` struct

Encapsulates a single unit of work.

```c
typedef struct task_t
{
    task_function_t exe_function;
    arg_free_t      arg_free;
    void *          arg;
} task_t;
```

#### `thread_pool_t` struct

The internal thread pool structure.

```c
typedef struct thread_pool
{
    size_t      thread_count;
    queue_t *   task_queue;
    pthread_t * worker_threads;
} thread_pool_t;
```

---

### Core Behavior

#### `thread_pool_create()`

- Allocates and initializes a `thread_pool_t` struct.
- Initializes a shared, unbounded `queue_t` for tasks.
- Starts the specified number of threads using `pthread_create`.

#### `threadpool_add_task()`

- Wraps the task function and its argument in a `task_t` structure.
- Enqueues the task using `queue_enqueue()` (which blocks if bounded and full).

#### Worker Thread (`worker_thread()`)

- Each worker thread repeatedly dequeues tasks from the task queue.
- If the global `signal_flag` is set to `SHUTDOWN`, the loop terminates.
- If a cleanup function is provided, it is called after task execution.

#### `thread_pool_shutdown()`

- Joins each thread using `pthread_join()` after a shutdown signal is detected.

#### `thread_pool_destroy()`

- Performs shutdown if needed.
- Destroys the task queue.
- Frees thread handles and the thread pool structure.

---

## Thread Safety

- The queue used internally handles synchronization.
- All workers block on `queue_dequeue()` and only proceed when new tasks are available.
- Shutdown is signaled externally using `signal_flag`, expected to be defined and modified by the caller.

---

## Usage Example

```c
#include "thread_pool.h"

void * print_task(void * arg)
{
    int *value = (int *)arg;
    printf("Task says: %d\n", *value);
    return NULL;
}

void cleanup_int(void * arg)
{
    free(arg);
}

int main(void)
{
    thread_pool_t * pool = thread_pool_create(4);
    if (!pool)
    {
        fprintf(stderr, "Failed to create thread pool.\n");
        return 1;
    }

    for (int i = 0; i < 10; ++i)
    {
        int *data = malloc(sizeof(int));
        *data = i;
        threadpool_add_task(pool, print_task, cleanup_int, data);
    }

    // Signal shutdown (external signal handler must set signal_flag = SHUTDOWN)
    signal_flag = SHUTDOWN;

    thread_pool_shutdown(pool);
    thread_pool_destroy(&pool);

    return 0;
}
```

---

## Notes

- The thread pool assumes a global `signal_flag` (e.g., set by a signal handler).
- Task arguments must be heap-allocated if you intend to use `arg_free`.
- Calling `threadpool_add_task()` after shutdown will fail silently.
- Do not destroy the thread pool before calling `thread_pool_shutdown()`.
