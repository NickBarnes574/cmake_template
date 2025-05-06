#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "signal_handler.h"
#include "thread_pool.h"
#include "utilities.h"

/**
 * @brief A task wrapper structure for the thread pool
 *
 */
typedef struct task_t
{
    task_function_t exe_function; // The function to execute
    arg_free_t      arg_free;     // Optional cleanup for argument
    void *          arg;          // Argument to the function
} task_t;

/**
 * @brief A struct for a thread_pool
 *
 */
typedef struct thread_pool
{
    size_t      thread_count;   // Total threads in the pool
    queue_t *   task_queue;     // Shared task queue
    pthread_t * worker_threads; // Thread handles
} thread_pool_t;

/**
 * @brief Worker thread routine for processing tasks in the thread pool
 *
 * @param thread_pool Pointer to the thread pool
 * @return void* Always returns NULL
 */
static void * thread_routine(void * data);

/**
 * @brief Allocates and initializes a new task
 *
 * @param exe_function Task execution function (must not be NULL)
 * @param arg_free Optional cleanup function for the argument
 * @param arg Argument to be passed to the task function (can be NULL)
 * @return task_t* Pointer to the allocated task, or NULL on failure
 */
static task_t * create_task(task_function_t exe_function,
                            arg_free_t      arg_free,
                            void *          arg);

/**
 * @brief Executes a task and performs cleanup
 *
 * @param task Pointer to the task
 * @return int E_SUCCESS on success, E_FAILURE on failure
 */
static int process_task(task_t * task);

thread_pool_t * thread_pool_create(size_t thread_count)
{
    thread_pool_t * thread_pool = NULL;
    int             exit_code   = E_FAILURE;

    if (MIN_THREADS > thread_count)
    {
        PRINT_DEBUG("thread_pool_create(): Invalid thread_count.\n");
        goto END;
    }

    thread_pool = calloc(1, sizeof(thread_pool_t));
    if (NULL == thread_pool)
    {
        PRINT_DEBUG("thread_pool_create(): CMR failure - thread_pool.\n");
        goto END;
    }

    thread_pool->task_queue = queue_init(free, QUEUE_UNBOUNDED);
    if (NULL == thread_pool->task_queue)
    {
        PRINT_DEBUG("thread_pool_create(): Unable to initialize task queue.\n");
        goto CLEANUP_THREAD_POOL;
    }

    thread_pool->thread_count   = thread_count;
    thread_pool->worker_threads = calloc(thread_count, sizeof(pthread_t));
    if (NULL == thread_pool->worker_threads)
    {
        PRINT_DEBUG("thread_pool_create(): CMR failure - threads.\n");
        goto CLEANUP_TASK_QUEUE;
    }

    for (size_t idx = 0; idx < thread_count; idx++)
    {
        exit_code = pthread_create(&thread_pool->worker_threads[idx],
                                   NULL,
                                   thread_routine,
                                   thread_pool);
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("thread_pool_create(): pthread_create() failed.\n");
            thread_pool_shutdown(thread_pool);
            thread_pool_destroy(&thread_pool);
            goto END;
        }
    }

    goto END;

CLEANUP_TASK_QUEUE:
    queue_destroy(&thread_pool->task_queue);
CLEANUP_THREAD_POOL:
    free(thread_pool);
    thread_pool = NULL;
END:
    return thread_pool;
}

int thread_pool_add_task(thread_pool_t * thread_pool,
                         task_function_t task,
                         arg_free_t      arg_free,
                         void *          arg)
{
    int      exit_code = E_FAILURE;
    task_t * new_task  = NULL;

    if ((NULL == thread_pool) || (NULL == task))
    {
        PRINT_DEBUG("thread_pool_add_task(): NULL argument passed.");
        goto END;
    }

    if (signal_flag == SHUTDOWN)
    {
        PRINT_DEBUG("thread_pool_add_task(): Signal shutdown in progress.");
        goto END;
    }

    new_task = create_task(task, arg_free, arg);
    if (NULL == new_task)
    {
        PRINT_DEBUG("thread_pool_add_task(): Failed to create task.");
        goto END;
    }

    exit_code = queue_enqueue(thread_pool->task_queue, new_task);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("thread_pool_add_task(): Failed to enqueue task.");
        free(new_task);
        new_task = NULL;
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int thread_pool_shutdown(thread_pool_t * thread_pool)
{
    int exit_code = E_FAILURE;

    if (NULL == thread_pool)
    {
        PRINT_DEBUG("thread_pool_shutdown(): NULL argument passed.\n");
        goto END;
    }

    // Wake up all waiting threads
    pthread_mutex_lock(&thread_pool->task_queue->mutex);
    pthread_cond_broadcast(&thread_pool->task_queue->not_empty);
    pthread_mutex_unlock(&thread_pool->task_queue->mutex);

    for (size_t idx = 0; idx < thread_pool->thread_count; ++idx)
    {
        pthread_join(thread_pool->worker_threads[idx], NULL);
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int thread_pool_destroy(thread_pool_t ** thread_pool)
{
    int exit_code = E_FAILURE;

    if ((NULL == thread_pool) || (NULL == *thread_pool))
    {
        PRINT_DEBUG("thread_pool_destroy(): NULL thread_pool passed.\n");
        goto END;
    }

    thread_pool_shutdown(*thread_pool);

    queue_destroy(&(*thread_pool)->task_queue);
    free((*thread_pool)->worker_threads);
    (*thread_pool)->worker_threads = NULL;
    free(*thread_pool);
    *thread_pool = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void * thread_routine(void * data)
{
    thread_pool_t * thread_pool = (thread_pool_t *)data;
    task_t *        task        = NULL;

    while (signal_flag != SHUTDOWN)
    {
        task = queue_dequeue(thread_pool->task_queue);
        if (NULL != task)
        {
            process_task(task);
            free(task);
        }
    }

    return NULL;
}

static task_t * create_task(task_function_t exe_function,
                            arg_free_t      arg_free,
                            void *          arg)
{
    task_t * new_task = NULL;

    if (NULL == exe_function) // arg can be NULL
    {
        PRINT_DEBUG("create_task(): NULL argument passed.\n");
        goto END;
    }

    new_task = calloc(1, sizeof(task_t));
    if (NULL == new_task)
    {
        PRINT_DEBUG("create_task(): CMR failure - new_task.\n");
        goto END;
    }

    new_task->exe_function = exe_function;
    new_task->arg_free     = arg_free;
    new_task->arg          = arg;

END:
    return new_task;
}

static int process_task(task_t * task)
{
    int exit_code = E_FAILURE;

    if (NULL == task)
    {
        PRINT_DEBUG("process_task(): NULL argument passed.\n");
        goto END;
    }
    if (NULL == task->exe_function)
    {
        PRINT_DEBUG("process_task(): NULL task function; unable to execute.\n");
        goto END;
    }

    task->exe_function(task->arg);

    if (NULL != task->arg_free)
    {
        task->arg_free(task->arg);
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
