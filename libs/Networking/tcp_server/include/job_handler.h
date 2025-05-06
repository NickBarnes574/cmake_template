/**
 * @file job_handler.h
 * @brief Job handler module for processing client requests.
 */

#ifndef _JOB_HANDLER_H
#define _JOB_HANDLER_H

#include <pthread.h> // pthread_mutex_t
#include <stdint.h>  // uint8_t

#include "socket_manager.h"

/**
 * @struct job_arg
 * @brief Structure to hold arguments for a job.
 *
 * This structure contains the necessary arguments for processing a job,
 * including the client file descriptor, a mutex for synchronizing access to
 * the file descriptor, and a socket manager.
 */
typedef struct job_arg
{
    int                client_fd; // Client file descriptor.
    pthread_mutex_t *  fd_mutex;  // Mutex pointer for the client fd.
    socket_manager_t * sock_mgr;  // Pointer to the socket manager.
} job_arg_t;

/**
 * @brief Process a job for a given client.
 *
 * This function processes the job for the specified client file descriptor.
 *
 * @param client_fd The file descriptor of the client.
 * @return int Status code, E_SUCCESS on success, or an error code on failure.
 */
int process_job(int client_fd);

/**
 * @brief Process a client request in a separate thread.
 *
 * This function processes the client request provided in the argument, which
 * includes the client file descriptor and other necessary information.
 *
 * @param arg Pointer to the argument structure containing job details.
 * @return void* Always returns NULL.
 */
void * process_client_request(void * arg);

#endif /* _JOB_HANDLER_H */

/*** end of file ***/
