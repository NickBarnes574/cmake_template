#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "request_handler.h"
#include "utilities.h"

int create_job_args(int               client_fd,
                    uint8_t           opcode,
                    pthread_mutex_t * fd_mutex,
                    job_arg_t **      job_args)
{
    int         exit_code    = E_FAILURE;
    job_arg_t * new_job_args = NULL;

    if ((NULL == fd_mutex) || (NULL == job_args))
    {
        print_error("create_job_args(): NULL argument passed.");
        goto END;
    }

    new_job_args = calloc(1, sizeof(job_arg_t));
    if (NULL == new_job_args)
    {
        print_error("create_job_args(): CMR failure - new_job_args.");
        goto END;
    }

    new_job_args->client_fd = client_fd;
    new_job_args->opcode    = opcode;
    new_job_args->fd_mutex  = fd_mutex;

    *job_args = new_job_args;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void free_job_args(void * arg)
{
    job_arg_t * job_args = NULL;

    if (NULL == arg)
    {
        print_error("free_job_arg(): NULL argument passed.");
        goto END;
    }

    job_args = (job_arg_t *)arg;

    free(job_args);

END:
    return;
}

void * process_client_request(void * arg)
{
    job_arg_t * job_args = NULL;

    if (NULL == arg)
    {
        print_error("process_client_request(): NULL argument passed.");
        goto END;
    }

    job_args = (job_arg_t *)arg;

    pthread_mutex_lock(job_args->fd_mutex);

    // Process data
    printf("Processing data: %d\n", job_args->opcode);

    // TODO: Add processing logic based on opcode here

    pthread_mutex_unlock(job_args->fd_mutex);

END:
    return NULL;
}

/*** end of file ***/
