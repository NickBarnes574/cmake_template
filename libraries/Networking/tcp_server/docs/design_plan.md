# TCP Server (Combination of Poll and Thread Pool)

## Overview

The primary goal of combining `poll()` with a thread pool is to efficiently manage multiple client connections and handle tasks concurrently without creating and destroying threads repeatedly. This setup balances the efficiency of non-blocking I/O with the scalability of multi-threading.

### Components

1. **Main Server Loop (`poll()`):** This is where the server listens for events (like incoming data or new connections) on a set of sockets. `poll()` is used to monitor these sockets and determine when they're ready for I/O operations without blocking the server.
2. **Thread Pool:** A fixed number of threads that handle heavy lifting tasks such as processing data, performing computations, and preparing responses. These threads are pre-created to avoid the overhead associated with dynamic thread creation and destruction.

### Workflow

#### 1. Initialization

- **Start the Server:** Initialize and bind the server socket, then listen for incoming connections.
- **Setup `poll()`:** Register the server socket and any other initial file descriptors (sockets) with the `poll()` instance.
- **Thread Pool Creation:** Initialize a pool of worker threads. These threads initially wait for tasks to be assigned.

#### 2. Running the Server

- **`poll()` Monitoring:** The server continuously calls `poll()` to check for any activity on the monitored sockets. The `poll()` function will block until there are events (like incoming connections or data ready to be read).
  
- **Event Handling:**
  - **New Connections:** When `poll()` indicates a new connection, the server accepts this connection and adds the new socket to the `poll()` list to monitor further events.
  - **Data Reception:** If data is ready to be read on any socket, the main loop retrieves this data but does not process it. Instead, it packages the data along with the socket descriptor into a task.
  
- **Task Dispatch to Thread Pool:**
  - **Creating Tasks:** Tasks created from events (such as data to process) are enqueued into a task queue.
  - **Task Processing:** Worker threads in the pool pick up these tasks from the queue, process them (e.g., compute a response based on the received data), and prepare a response.
  
- **Sending Responses:**
  - **Direct Response:** Depending on the design, worker threads might directly send responses to clients if it's safe to do so without additional synchronization.
  - **Response Queue:** Alternatively, processed responses might be handed back to the main thread or a designated I/O thread to send back to clients, particularly when managing responses must avoid race conditions.

#### 3. Resource Management

- **Connection Closure:** Handle client disconnections or errors by closing sockets and removing them from the `poll()` list.
- **Resource Cleanup:** Ensure proper cleanup of all resources, such as closed sockets and completed tasks, to prevent leaks and ensure system stability.

### Benefits of the Design

- **Efficiency:** `poll()` efficiently handles multiple connections by blocking until necessary, reducing CPU usage compared to constant polling.
- **Concurrency:** The thread pool allows multiple tasks to be processed in parallel, enhancing throughput and responsiveness.
- **Scalability:** Managing the number of threads in the pool helps control resource usage, making the server scalable under varying loads.

### Conclusion

This model combines the I/O efficiency of `poll()` with the processing power of a thread pool to create a scalable, high-performance TCP server capable of handling many client connections and tasks simultaneously.

## Signal Handling

Handling signal interruptions, especially SIGINT (Ctrl+C), is important for gracefully shutting down a server while ensuring data integrity and proper disconnection of clients. Handling SIGINT properly ensures that the server remains robust and reliable, even during unexpected terminations.

1. **Graceful Shutdown**: Upon receiving SIGINT, the server should:
   - Stop accepting new connections immediately.
   - Finish processing any ongoing tasks.
   - Send notifications to connected clients about the impending shutdown if applicable.
   - Properly close all client connections.
   - Clean up resources (e.g., close sockets, release memory).
   - Exit cleanly.

2. **Data Integrity**: Ensure that any data being processed at the time of the signal is handled correctly:
   - Complete any transactions or data processing that can be finished quickly.
   - Roll back or safely terminate any operations that cannot be completed.

3. **Client Notification**: If your server protocol supports it, notify clients that the server is shutting down, so they can gracefully disconnect or reconnect later.

### Implementing Signal Handling in a TCP Server

To implement this behavior in a C-based TCP server using `poll()` and a thread pool, you can follow these steps:

#### Step 1: Register Signal Handler

- **Set Up Signal Handling**: Use `signal()` or preferably `sigaction()` to set up a handler for SIGINT. `sigaction()` is recommended for its reliability and service under various UNIX systems.

```c
#include <signal.h>

void sigint_handler(int sig) {
    // Set a flag indicating the server should shut down
}

struct sigaction sa;
sa.sa_handler = sigint_handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
sigaction(SIGINT, &sa, NULL);
```

#### Step 2: Main Loop Modification

- **Check for Shutdown Flag**: In your server’s main loop, check for a flag that gets set by your SIGINT handler. When this flag is set, break out of the loop to start the shutdown process.

```c
volatile sig_atomic_t shutdown_flag = 0;

void sigint_handler(int sig) {
    shutdown_flag = 1;
}

// In your main loop
while (!shutdown_flag) {
    // Continue normal operation
}
```

#### Step 3: Graceful Shutdown

- **Complete Current Tasks**: Allow worker threads to finish their current tasks if possible. Avoid dispatching new tasks to the thread pool.
- **Close Client Connections**: Gracefully close all active client sockets.
- **Resource Cleanup**: Free any allocated resources, such as memory and open files.

#### Step 4: Worker Thread Management

- **Inform Threads**: Use a condition variable or another signaling mechanism to inform all worker threads that a shutdown is in progress.
- **Wait for Threads to Finish**: Use `pthread_join()` or a similar function to wait for all worker threads to complete their tasks and terminate.

```c
for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
}
```

#### Step 5: Exit

- **Close Server Socket**: Ensure the main server socket is closed to prevent new connections.
- **Exit the Program**: Exit your program with an appropriate status code.
