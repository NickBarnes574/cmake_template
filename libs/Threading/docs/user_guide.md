# Thread Pool Module: User Guide

This user guide describes how to use the `thread_pool.h` API to work with a fixed-size thread pool that executes asynchronous tasks. It is designed for developers integrating the thread pool into multi-threaded applications and assumes access to the public interface only.

---

## What is the Thread Pool Module?

The thread pool module provides a C interface to manage a reusable pool of threads that execute user-defined tasks concurrently. Tasks are submitted to a shared, thread-safe queue and picked up by available worker threads.

---

## Key Features

- Fixed-size thread pool using POSIX threads
- Generic task function interface (`void * (*)(void *)`)
- Optional custom cleanup (`arg_free_t`) for dynamically allocated task data
- Internally thread-safe via synchronized task queue
- Works with `signal_flag` for graceful shutdown

---

## Getting Started

### Include the Header

```c
#include "thread_pool.h"
```

Ensure `thread_pool.h` is in your include path.

### Create a Thread Pool

```c
thread_pool_t *pool = thread_pool_create(4);
```

- `thread_count` must be `>= MIN_THREADS` (default minimum is 2).
- Returns a pointer to the pool on success, or `NULL` on failure.

---

## Submitting Tasks

### Add a Task

```c
int threadpool_add_task(thread_pool_t *pool,
                        task_function_t task,
                        arg_free_t cleanup,
                        void *arg);
```

- `task`: Function to run asynchronously.
- `cleanup`: Optional cleanup function for `arg` (can be `NULL`).
- `arg`: Argument to pass to the task (can be `NULL`).
- Returns `E_SUCCESS` or `E_FAILURE`.

**Note:** If the global `signal_flag == SHUTDOWN`, the task will not be enqueued.

---

## Lifecycle Management

### Shutdown the Pool

```c
int thread_pool_shutdown(thread_pool_t *pool);
```

- Waits for all worker threads to finish their current tasks and exit.
- Should be called after setting `signal_flag = SHUTDOWN`.

### Destroy the Pool

```c
int thread_pool_destroy(thread_pool_t **pool);
```

- Frees all memory and deallocates internal state.
- Must be called after shutdown.
- Sets `*pool` to `NULL`.

---

## Example Use Cases

### Basic Example

```c
#include "thread_pool.h"

void *print_task(void *arg)
{
    int *val = (int *)arg;
    printf("Task says: %d\n", *val);
    return NULL;
}

void cleanup_int(void *arg)
{
    free(arg);
}

int main(void)
{
    thread_pool_t *pool = thread_pool_create(4);
    if (!pool)
    {
        fprintf(stderr, "Failed to create thread pool.\n");
        return -1;
    }

    for (int i = 0; i < 10; ++i)
    {
        int *value = malloc(sizeof(int));
        *value = i;
        threadpool_add_task(pool, print_task, cleanup_int, value);
    }

    // Trigger global shutdown signal (assumes signal_flag is shared)
    signal_flag = SHUTDOWN;

    thread_pool_shutdown(pool);
    thread_pool_destroy(&pool);
    return 0;
}
```

---

## Best Practices

- Always call `thread_pool_shutdown()` before destroying the pool.
- Use `arg_free_t` to avoid leaks from dynamically allocated task arguments.
- Tasks should be short-lived and non-blocking whenever possible.
- Never submit tasks after initiating shutdown (they will be ignored).
- Handle synchronization of shared resources *inside* your task logic.

---

## Thread Safety

All public API functions are thread-safe and designed to be used concurrently.

- `threadpool_add_task()` safely enqueues tasks.
- Worker threads block on the internal queue until tasks are available.
- Shutdown coordination relies on an external `signal_flag` (`ACTIVE` or `SHUTDOWN`).
