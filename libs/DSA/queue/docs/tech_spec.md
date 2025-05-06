# Queue Module: Technical Documentation

This document outlines the design, structure, and usage of the `queue.c` and `queue.h` files. This queue implementation is a thread-safe, generic FIFO queue that optionally supports bounded capacity and condition variable signaling for producer-consumer style blocking.

---

## Overview

The queue supports the following features:

- Generic data storage (void pointers)
- Optional capacity limits (bounded or unbounded)
- Internal synchronization with `pthread_mutex_t`
- Blocking operations using `pthread_cond_t not_empty` and `not_full`
- User-defined free functions for complex types

---

## Header File: `queue.h`

### Constants

```c
#define QUEUE_UNBOUNDED        0
#define QUEUE_DEFAULT_MAX_SIZE 64
```

These constants define the default behavior:

- `QUEUE_UNBOUNDED` means the queue has no maximum capacity.
- `QUEUE_DEFAULT_MAX_SIZE` is a conventional upper limit when one is desired.

### Data Structures

#### `queue_node_t`

Represents a node in the queue.

```c
typedef struct queue_node_t {
    void *data;
    struct queue_node_t *next;
} queue_node_t;
```

#### `FREE_F`

A function pointer type to allow custom `free()` functions for queue contents.

```c
typedef void (*FREE_F)(void *);
```

#### `queue_t`

The main queue structure.

```c
typedef struct queue_t {
    uint32_t current_size;
    uint32_t max_size;
    queue_node_t *head;
    queue_node_t *tail;
    FREE_F customfree;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} queue_t;
```

### API Functions

#### Initialization and Destruction

```c
queue_t * queue_init(FREE_F customfree, uint32_t max_size);
void queue_destroy(queue_t ** queue_addr);
```

- Initializes a queue with optional capacity and optional custom free function.
- Destroys and deallocates the queue, releasing resources.

#### Status Checks

```c
bool queue_is_empty(queue_t * queue);
bool queue_is_full(queue_t * queue);
```

- Thread-safe checks for current queue state.

#### Core Operations

```c
int queue_enqueue(queue_t * queue, void * data);
void * queue_dequeue(queue_t * queue);
void * queue_peek(queue_t * queue);
int queue_clear(queue_t * queue);
```

- `enqueue` blocks if full.
- `dequeue` blocks if empty.
- `peek` retrieves the front item without removing it.
- `clear` removes all elements and calls the user-defined `customfree`.

---

## Source File: `queue.c`

### Key Function Behavior

#### `queue_enqueue()`

- Locks queue mutex
- Waits on `not_full` if queue is full
- Allocates and appends a new node
- Signals `not_empty` to wake consumers

#### `queue_dequeue()`

- Locks queue mutex
- Waits on `not_empty` if queue is empty
- Removes and frees head node
- Signals `not_full` to wake producers

#### `queue_clear()`

- Loops through all nodes, calling `queue_dequeue()` and `customfree()`

#### `queue_destroy()`

- Calls `queue_clear()`
- Destroys mutex and condition variables
- Frees queue memory

### Thread Safety

- All access to shared state is protected by `queue->mutex`.
- Blocking semantics are handled with `pthread_cond_t` signaling.

---

## Usage Example

```c
#define E_FAILURE (-1)
#define E_SUCCESS 0
#define QUEUE_SIZE 10
#define MEANING_OF_LIFE 42

int test_queue()
{
    int exit_code = E_FAILURE;
    int *item = NULL;
    int *result = NULL;
    void *data = NULL;
    queue_t *queue = NULL;

    queue = queue_init(NULL, QUEUE_SIZE);
    if (NULL == queue)
    {
        PRINT_DEBUG("test_queue(): Queue initialization failure.\n");
        goto END;
    }

    item = calloc(1, sizeof(int));
    if (NULL == item)
    {
        PRINT_DEBUG("test_queue(): CMR failure - item.\n");
        goto END;
    }

    *item = MEANING_OF_LIFE;

    exit_code = queue_enqueue(queue, item);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("test_queue(): queue_enqueue() failure.\n");
        goto END;
    }

    data = queue_dequeue(queue);
    if (NULL == data)
    {
        PRINT_DEBUG("test_queue(): NULL data dequeued.\n");
        goto END;
    }

    result = (*int)data;

    printf("Dequeued: %d\n", *result);
    free(result);
    result = NULL;

    queue_destroy(&queue);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}
```

---

## Notes

- Always `free()` data dequeued from the queue unless reused.
- Be sure to call `queue_destroy()` to avoid memory leaks.
- Custom types should use a proper `customfree()` to avoid partial leaks.
