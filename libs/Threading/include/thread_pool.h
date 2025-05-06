#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "queue.h"
#include <stdlib.h>

#define MIN_THREADS ((size_t)2)

/**
 * @brief Task function type for thread pool workers.
 *
 * Each task submitted to the thread pool must conform to this signature.
 * The argument can be NULL and should be cast internally to the expected type.
 */
typedef void * (*task_function_t)(void * arg);

/**
 * @brief Optional cleanup function for task arguments.
 *
 * If provided, this function will be called after the task is executed,
 * to clean up dynamically allocated memory or other resources. It must
 * safely handle NULL pointers.
 */
typedef void (*arg_free_t)(void * arg);

/**
 * @brief Opaque structure representing a thread pool.
 *
 * The internal structure is hidden from the user.
 */
typedef struct thread_pool thread_pool_t;

/**
 * @brief Create and initialize a thread pool.
 *
 * @param thread_count The number of worker threads to spawn.
 *
 * @return A pointer to a valid thread_pool_t instance on success,
 *         or NULL on failure.
 */
thread_pool_t * thread_pool_create(size_t thread_count);

/**
 * @brief Gracefully shut down the thread pool.
 *
 * Waits for all tasks to complete and joins all worker threads.
 * No new tasks will be accepted once shutdown begins.
 *
 * @param thread_pool Pointer to the thread pool to shut down.
 *
 * @return E_SUCCESS on success, E_FAILURE on failure.
 */
int thread_pool_shutdown(thread_pool_t * thread_pool);

/**
 * @brief Destroy the thread pool and free all resources.
 *
 * Performs a shutdown if not already done, clears the task queue,
 * joins all threads, and frees memory.
 *
 * @param thread_pool Pointer to the thread pool pointer to destroy.
 *                    Will be set to NULL after cleanup.
 *
 * @return E_SUCCESS on success, E_FAILURE on failure.
 */
int thread_pool_destroy(thread_pool_t ** thread_pool);

/**
 * @brief Submit a new task to the thread pool.
 *
 * @param thread_pool The thread pool to submit to.
 * @param task The function to execute in the thread.
 * @param arg_free An optional cleanup function for the task argument.
 * @param arg The argument to pass to the task function.
 *
 * @note The task function must not be NULL.
 *       The arg can be NULL.
 *       The arg_free function can be NULL if no cleanup is required.
 *
 * @return E_SUCCESS on success, E_FAILURE on failure.
 */
int thread_pool_add_task(thread_pool_t * thread_pool,
                         task_function_t task,
                         arg_free_t      arg_free,
                         void *          arg);

#endif /* _THREAD_POOL_H */

/*** end of file ***/
