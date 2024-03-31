#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdlib.h>

#include "queue.h"

#define MIN_THREADS (size_t)2

/**
 * @brief A thread job function. The threadpool should operate on a job of this
 * type.
 * @note JOB_F must know and validate the internals of its singular argument,
 * void * arg. arg may be NULL.
 */
typedef void *(*JOB_F)(void *arg);

/**
 * @brief a custom free function for an arg
 * @note FREE_F must be safe if a NULL pointer is provided to it.
 */
typedef void (*FREE_F)(void *data);

/**
 * @brief A threadpool type. Internals to be implemented by trainee.
 */
typedef struct threadpool threadpool_t;

/**
 * @brief Create a new threadpool and instantiate as required.
 *
 * @param thread_count The number of threads to create in the threadpool
 *
 * @return SUCCESS: A threadpool instance of type threadpool_t.
 *         FAILURE: NULL
 */
threadpool_t *threadpool_create(size_t thread_count);

/**
 * @brief Nice shutdown of threadpool. Do not take any more work.
 * Finish the work that has already been accepted.
 *
 * @param pool_p A valid threadpool instance
 *
 * @return SUCCESS: SUCCESS
 *         FAILURE: ERROR
 */
int threadpool_shutdown(threadpool_t *pool_p);

/**
 * @brief Destroy a threadpool. Clean up all resources and memory.
 *
 * @param pool_pp A threadpool to be destroyed. Will be set to NULL upon
 * successful destruction.
 *
 * @return SUCCESS: SUCCESS
 *         FAILURE: ERROR
 */
int threadpool_destroy(threadpool_t **pool_pp);

/**
 * @brief Add a job to to the threadpool to work on.
 *
 * @param pool_p The valid pool to execute the job.
 * @param job The job to be executed by the pool.
 * @param del_f A user defined function to free and clean up arg_p, if not
 * required, set to NULL. Internal structure of arg will be known to the del_f,
 * and it must safely handle NULL inputs.
 * @param arg_p The argument(s) required by the job, if any. Internal structure
 * of arg will be known to the job.
 *
 * @note A valid job must include the function pointer job_f. The job may not
 * include an arg and thus NULL must be accepted as an arg. The free_f may also
 * be NULL.
 *
 * @return SUCCESS: SUCCESS
 *         FAILURE: ERROR
 */
int threadpool_add_job(threadpool_t *pool_p,
                       JOB_F job,
                       FREE_F del_f,
                       void *arg_p);

#endif
