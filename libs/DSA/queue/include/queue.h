#ifndef _QUEUE_H
#define _QUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define QUEUE_UNBOUNDED        0
#define QUEUE_DEFAULT_MAX_SIZE 64

/**
 * @brief structure of a queue node
 *
 * @param data void pointer to whatever data that queue points to
 */
typedef struct queue_node_t
{
    void *                data;
    struct queue_node_t * next;
} queue_node_t;

/**
 * @brief A pointer to a user-defined free function. This is used to free
 *        memory allocated for queue data. For simple data types, this is
 *        just a pointer to the standard free function. More complex structs
 *        stored in queues may require a function that calls free on multiple
 *        components.
 *
 */
typedef void (*FREE_F)(void *);

/**
 * @brief structure of a queue object
 *
 * @param current_size is the number of nodes the queue is currently storing
 * @param max_size is the maximum capacity of the queue (0 = unbounded)
 * @param head is the pointer to the front of the queue
 * @param tail is the pointer to the rear of the queue
 * @param customfree is a FREE_F pointer to a user defined free function
 * @param mutex protects the queue structure
 * @param not_empty signaled when queue transitions from empty to non-empty
 * @param not_full signaled when queue transitions from full to not-full
 */
typedef struct queue_t
{
    uint32_t        current_size;
    uint32_t        max_size;
    queue_node_t *  head;
    queue_node_t *  tail;
    FREE_F          customfree;
    pthread_mutex_t mutex;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;
} queue_t;

/**
 * @brief creates a new queue
 *
 * @param customfree pointer to user defined free function
 * @param max_size maximum size of the queue (use QUEUE_UNBOUNDED for no limit)
 * @note if the user passes in NULL, the queue should default to using free()
 * @returns the queue on success, NULL on failure
 */
queue_t * queue_init(FREE_F customfree, uint32_t max_size);

/**
 * @brief checks if the queue is empty
 *
 * @param queue pointer queue object
 * @return true if empty, false otherwise
 */
bool queue_is_empty(queue_t * queue);

/**
 * @brief checks if the queue is full
 *
 * @param queue pointer queue object
 * @return true if full, false otherwise
 */
bool queue_is_full(queue_t * queue);

/**
 * @brief pushes a new node into the queue
 *
 * @param queue pointer to queue pointer to push the node into
 * @param data data to be pushed into node
 * @return the 0 on success, non-zero value on failure
 */
int queue_enqueue(queue_t * queue, void * data);

/**
 * @brief pops the front node out of the queue
 *
 * @param queue pointer to data to dequeue
 * @return the data of the front node on success or NULL on failure
 */
void * queue_dequeue(queue_t * queue);

/**
 * @brief get the data from the node at the front of the queue without popping
 *
 * @param queue pointer to data to peek
 * @return the pointer to the head on success or NULL for failure
 */
void * queue_peek(queue_t * queue);

/**
 * @brief clear all nodes out of a queue
 *
 * @param queue pointer to queue pointer to clear out
 * @return the 0 on success, non-zero value on failure
 */
int queue_clear(queue_t * queue);

/**
 * @brief delete a queue
 *
 * @param queue_addr pointer to address of queue to be destroyed
 * @return the 0 on success, non-zero value on failure
 */
void queue_destroy(queue_t ** queue_addr);

#endif
