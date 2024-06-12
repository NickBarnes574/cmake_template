/**
 * @file job_handler.h
 *
 * @brief
 */
#ifndef _JOB_HANDLER_H
#define _JOB_HANDLER_H

#include <pthread.h> // pthread_mutex_t
#include <stdint.h>  // uint8_t

#include "socket_manager.h"

typedef struct job_arg
{
    int                client_fd; // Client file descriptor.
    pthread_mutex_t *  fd_mutex;  // Mutex pointer for the client fd.
    socket_manager_t * sock_mgr;
} job_arg_t;

void * process_client_request(void * arg);

#endif /* _JOB_HANDLER_H */

/*** end of file ***/
