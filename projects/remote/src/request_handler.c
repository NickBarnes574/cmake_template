#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "job_handler.h"
#include "utilities.h"

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
