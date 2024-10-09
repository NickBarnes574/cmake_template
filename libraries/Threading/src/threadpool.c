#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "signal_handler.h"
#include "threadpool.h"
#include "utilities.h"

#define ACTIVATE     1 // Activate the threadpool
#define SHUTDOWN     0 // Shutdown the threadpool
#define EMPTY        0 // Work queue is empty
#define NOT_EMPTY    1 // Work queue is not empty
#define KEEP_RUNNING 0 // Default signal for the signal handler

/**
 * @brief A struct for a job
 *
 */
typedef struct job
{
    JOB_F  job;    // The job to perform
    FREE_F del_f;  // The custom free function for the job
    void * args_p; // The arguments for the job
} job_t;

/**
 * @brief A struct for a threadpool
 *
 */
typedef struct threadpool
{
    size_t          thread_count; // The number of current threads
    size_t          max_threads;  // The maximum number of threads
    queue_t *       job_queue;    // A job queue
    pthread_t *     threads;      // The thread list
    pthread_mutex_t mutex;        // The mutex for a queue
    pthread_cond_t  condition;    // Used for signaling threads
    bool work_mutex_initialized;  // States if work mutex has been initialized
    bool queue_mutex_initialized; // States if queue mutex has been initialized
    bool condition_initialized;   // States if condition has been initialized
    sig_atomic_t signal;          // A shutdown signal for the threadpool ON/OFF
} threadpool_t;

/**
 * @brief Initializes a threadpool by setting up a mutex, work condition, work
 * queue, and allocating threads.
 *
 * @param threadpool_p The threadpool to setup
 * @param thread_count The number of threads to create
 * @return int Returns 0 on success, -1 on failure
 */
static int threadpool_setup(threadpool_t * threadpool_p, size_t thread_count);

/**
 * @brief Uninitializes a threadpool if threadpool_setup() fails.
 *
 * @param threadpool_pp The address of a threadpool to be destroyed
 */
static void threadpool_teardown(threadpool_t ** threadpool_pp);

/**
 * @brief Used to start each thread in a threadpool.
 *
 * @param pool_p The threadpool to start threads in
 * @return void*
 */
static void * start_thread(void * pool_p);

/**
 * @brief Creates a new job for a thread.
 *
 * @param job The job to create
 * @param del_f The delete function
 * @param arg_p The argument to pass
 * @return job_t* Returns NULL on error
 */
static job_t * create_job(JOB_F job, FREE_F del_f, void * arg_p);

/**
 * @brief Waits for a new job.
 *
 * @param threadpool_p The threadpool to pass in
 * @return int Returns 0 on success, -1 on failure
 */
static int wait_for_job(threadpool_t * threadpool_p);

/**
 * @brief Gets the next job from a job queue.
 *
 * @param threadpool_p The threadpool to pass in
 * @param node_p The queue node to pass in
 * @param job_p The job to process
 * @return int Returns 0 on success, -1 on failure
 */
static int get_next_job(threadpool_t ** threadpool_p,
                        queue_node_t ** node_p,
                        job_t **        job_p);

/**
 * @brief Runs a job.
 *
 * @param job_p The job to run
 * @return int Returns 0 on success, -1 on failure
 */
static int process_job(job_t * job_p);

threadpool_t * threadpool_create(size_t thread_count)
{
    threadpool_t * threadpool_p = NULL;
    int            exit_code    = E_FAILURE;

    if (MIN_THREADS > thread_count)
    {
        print_error("threadpool_create(): Invalid thread_count.");
        goto END;
    }

    threadpool_p = calloc(1, sizeof(threadpool_t));
    if (NULL == threadpool_p)
    {
        print_error("threadpool_create(): 'threadpool_p' CMR failure.");
        goto END;
    }

    exit_code = threadpool_setup(threadpool_p, thread_count);
    if (E_SUCCESS != exit_code)
    {
        print_error("threadpool_create(): Unable to perform threadpool setup.");
        threadpool_teardown(&threadpool_p);
        free(threadpool_p);
        threadpool_p = NULL;
        goto END;
    }

    threadpool_p->signal       = ACTIVATE;
    threadpool_p->thread_count = thread_count;
    threadpool_p->max_threads  = thread_count;

    for (size_t idx = 0; idx < thread_count; idx++)
    {
        exit_code = pthread_create(
            &threadpool_p->threads[idx], NULL, start_thread, threadpool_p);
        if (E_SUCCESS != exit_code)
        {
            print_error("threadpool_create(): failed to create thread.");
            threadpool_teardown(&threadpool_p);
            free(threadpool_p);
            threadpool_p = NULL;
            goto END;
        }
    }

END:
    return threadpool_p;
}

int threadpool_shutdown(threadpool_t * pool_p)
{
    int exit_code = E_FAILURE;

    if (NULL == pool_p)
    {
        print_error("threadpool_shutdown(): NULL threadpool passed.");
        goto END;
    }

    pthread_mutex_lock(&signal_flag_mutex);
    pool_p->signal = SHUTDOWN;
    pthread_mutex_unlock(&signal_flag_mutex);

    pthread_mutex_lock(&pool_p->mutex);
    exit_code = pthread_cond_broadcast(&pool_p->condition);
    if (E_SUCCESS != exit_code)
    {
        print_error("threadpool_shutdown(): Unable to wake up threads.");
        pthread_mutex_unlock(&pool_p->mutex);
        goto END;
    }
    pthread_mutex_unlock(&pool_p->mutex);

    for (size_t idx = 0; idx < pool_p->thread_count; idx++)
    {
        exit_code = pthread_join(pool_p->threads[idx], NULL);
        if (E_SUCCESS != exit_code)
        {
            print_error("threadpool_shutdown(): Unable to join threads.");
            goto END;
        }
    }

    free(pool_p->threads);
    pool_p->threads = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int threadpool_destroy(threadpool_t ** pool_pp)
{
    int exit_code = E_FAILURE;

    if ((NULL == pool_pp) || (NULL == *pool_pp))
    {
        print_error("threadpool_destroy(): NULL threadpool passed.");
        goto END;
    }

    if (SHUTDOWN != (*pool_pp)->signal)
    {
        exit_code = threadpool_shutdown(*pool_pp);
        if (E_SUCCESS != exit_code)
        {
            print_error("threadpool_destroy(): Unable to perform shutdown.");
            goto END;
        }
    }

    threadpool_teardown(pool_pp);

    free(*pool_pp);
    *pool_pp = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int threadpool_add_job(threadpool_t * pool_p,
                       JOB_F          job,
                       FREE_F         del_f,
                       void *         arg_p)
{
    int     exit_code = E_FAILURE;
    job_t * new_job   = NULL;

    if ((NULL == pool_p) || (NULL == job))
    {
        print_error("threadpool_add_job(): NULL argument passed.");
        goto END;
    }

    if (SHUTDOWN == pool_p->signal)
    {
        print_error("threadpool_add_job(): Threadpool already shutdown.");
        goto END;
    }

    new_job = create_job(job, del_f, arg_p);
    if (NULL == new_job)
    {
        print_error("threadpool_add_job(): Unable to create job.");
        goto END;
    }

    pthread_mutex_lock(&pool_p->mutex);
    exit_code = queue_enqueue(pool_p->job_queue, new_job);
    if (E_SUCCESS != exit_code)
    {
        print_error("threadpool_add_job(): queue_enqueue() failed.");
        pthread_mutex_unlock(&pool_p->mutex);
        free(new_job);
        goto END;
    }
    pthread_cond_signal(&pool_p->condition);
    pthread_mutex_unlock(&pool_p->mutex);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int threadpool_setup(threadpool_t * threadpool_p, size_t thread_count)
{
    int exit_code = E_FAILURE;

    if (NULL == threadpool_p)
    {
        print_error("threadpool_setup(): NULL threadpool passed.");
        goto END;
    }

    // 1. Setup the mutex
    exit_code = pthread_mutex_init(&threadpool_p->mutex, NULL);
    if (E_SUCCESS != exit_code)
    {
        print_error("threadpool_create(): Unable to initialize work mutex.");
        goto END;
    }
    threadpool_p->work_mutex_initialized = true;

    // 2. Setup the work condition
    exit_code = pthread_cond_init(&threadpool_p->condition, NULL);
    if (E_SUCCESS != exit_code)
    {
        print_error("threadpool_create(): Unable to initialize condition.");
        goto END;
    }
    threadpool_p->condition_initialized = true;

    // 3. Setup the job queue
    threadpool_p->job_queue = queue_init(NULL);
    if (NULL == threadpool_p->job_queue)
    {
        print_error("threadpool_create(): Unable to initialize queue.");
        goto END;
    }

    // 4. Allocate memory for threads
    threadpool_p->threads = calloc(thread_count, sizeof(pthread_t));
    if (NULL == threadpool_p->threads)
    {
        print_error("threadpool_create(): 'threads' CMR failure.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void * start_thread(void * pool_p)
{
    // Initialize
    int            exit_code    = E_FAILURE;
    threadpool_t * threadpool_p = NULL;
    queue_node_t * node_p       = NULL;
    job_t *        job_p        = NULL;

    if (NULL == pool_p)
    {
        goto END;
    }

    threadpool_p = (threadpool_t *)pool_p;

    // Main loop for processing jobs
    for (;;)
    {
        pthread_mutex_lock(&threadpool_p->mutex);

        // Check for signal to stop running
        pthread_mutex_lock(&signal_flag_mutex);
        if (SHUTDOWN == threadpool_p->signal)
        {
            pthread_mutex_unlock(&threadpool_p->mutex);
            pthread_mutex_unlock(&signal_flag_mutex);
            break;
        }

        if (KEEP_RUNNING != signal_flag_g)
        {
            print_error("start_thread(): Signal caught.");
            pthread_mutex_unlock(&threadpool_p->mutex);
            pthread_mutex_unlock(&signal_flag_mutex);
            goto END;
        }
        pthread_mutex_unlock(&signal_flag_mutex);

        exit_code = wait_for_job(threadpool_p);
        if (E_SUCCESS != exit_code)
        {
            pthread_mutex_unlock(&threadpool_p->mutex);
        }

        exit_code = get_next_job(&threadpool_p, &node_p, &job_p);
        if (E_SUCCESS != exit_code)
        {
            pthread_mutex_unlock(&threadpool_p->mutex);
            goto END;
        }

        pthread_mutex_unlock(&threadpool_p->mutex);

        exit_code = process_job(job_p);
        if (E_SUCCESS != exit_code)
        {
            print_error("start_thread(): Unable to execute job.");
            goto END;
        }

        free(job_p);
        free(node_p);
    }

END:
    return NULL;
}

static job_t * create_job(JOB_F job, FREE_F del_f, void * arg_p)
{
    job_t * new_job = NULL;

    if (NULL == job)
    {
        print_error("threadpool_new_job(): NULL job passed.");
        goto END;
    }

    new_job = calloc(1, sizeof(job_t));
    if (NULL == new_job)
    {
        print_error("threadpool_new_job(): CMR failure.");
        goto END;
    }

    new_job->args_p = arg_p;
    new_job->job    = job;
    new_job->del_f  = del_f;

END:
    return new_job;
}

static int wait_for_job(threadpool_t * threadpool_p)
{
    int exit_code = E_FAILURE;

    if (NULL == threadpool_p)
    {
        print_error("wait_for_job(): NULL threadpool.");
        goto END;
    }

    while ((true == queue_is_empty(threadpool_p->job_queue)) &&
           (SHUTDOWN != threadpool_p->signal))
    {
        pthread_mutex_lock(&signal_flag_mutex);
        if (KEEP_RUNNING != signal_flag_g)
        {
            print_error("wait_for_job(): Signal caught.");
            pthread_mutex_unlock(&signal_flag_mutex);
            goto END;
        }
        pthread_mutex_unlock(&signal_flag_mutex);

        exit_code =
            pthread_cond_wait(&threadpool_p->condition, &threadpool_p->mutex);
        if (E_SUCCESS != exit_code)
        {
            print_error("Unable to wait on condition.");
            goto END;
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int get_next_job(threadpool_t ** threadpool_p,
                        queue_node_t ** node_p,
                        job_t **        job_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == threadpool_p) || (NULL == node_p) || (NULL == job_p))
    {
        print_error("get_next_job(): NULL argument passed.");
        goto END;
    }

    if ((SHUTDOWN == (*threadpool_p)->signal) &&
        (true == queue_is_empty((*threadpool_p)->job_queue)))
    {
        goto END;
    }

    *node_p = queue_dequeue((*threadpool_p)->job_queue);
    if (NULL == *node_p)
    {
        print_error("get_next_job(): NULL node.");
        goto END;
    }

    *job_p = (*node_p)->data;
    if (NULL == *job_p)
    {
        print_error("get_next_job(): NULL job.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int process_job(job_t * job_p)
{
    int exit_code = E_FAILURE;

    if (NULL == job_p)
    {
        print_error("process_job(): NULL job passed.");
        goto END;
    }

    if (NULL != job_p->job)
    {
        // Attempt to run the job
        job_p->job(job_p->args_p);
    }

    if (NULL != job_p->del_f)
    {
        // Attempt to perform cleanup if necessary
        job_p->del_f(job_p->args_p);
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void threadpool_teardown(threadpool_t ** threadpool_pp)
{
    if ((NULL == threadpool_pp) || (NULL == *threadpool_pp))
    {
        print_error("threadpool_destroy(): NULL threadpool passed.");
        goto END;
    }

    // 1. Deallocate memory for threads
    if (NULL != (*threadpool_pp)->threads)
    {
        free((*threadpool_pp)->threads);
    }

    // 2. Destroy the job queue
    if (NULL != (*threadpool_pp)->job_queue)
    {
        queue_destroy(&(*threadpool_pp)->job_queue);
    }

    // 3. Destroy the work condition
    if (true == (*threadpool_pp)->condition_initialized)
    {
        pthread_cond_destroy(&(*threadpool_pp)->condition);
    }

    // 4. Destroy the mutex
    if (true == (*threadpool_pp)->work_mutex_initialized)
    {
        pthread_mutex_destroy(&(*threadpool_pp)->mutex);
    }

END:
    return;
}
