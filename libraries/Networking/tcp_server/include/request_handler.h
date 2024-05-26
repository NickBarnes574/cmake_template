/**
 * @file request_handler.h
 *
 * @brief
 */
#ifndef _REQUEST_HANDLER_H
#define _REQUEST_HANDLER_H

#include <pthread.h> // pthread_mutex_t
#include <stdint.h>  // uint8_t

typedef struct job_arg
{
    uint8_t           opcode;    // Determines the type of request to make
    int               client_fd; // Client file descriptor
    pthread_mutex_t * fd_mutex;  // Mutex pointer for the client fd
} job_arg_t;

int    create_job_args(int               client_fd,
                       uint8_t           opcode,
                       pthread_mutex_t * fd_mutex,
                       job_arg_t **      job_args);
void   free_job_args(void * arg);
void * process_client_request(void * arg);

#endif /* _REQUEST_HANDLER_H */

/*** end of file ***/
