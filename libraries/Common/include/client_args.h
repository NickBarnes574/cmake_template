/**
 * @file client_args.h
 *
 * @brief
 */
#ifndef _CLIENT_ARGS_H
#define _CLIENT_ARGS_H

#include <pthread.h>
#include <stdint.h>

#include "hash_table.h"

typedef struct client_args client_args_t;

typedef int (*request_handler_t)(client_args_t * client_args);

struct client_args
{
    int               client_fd;
    request_handler_t handler_func;
    uint32_t *        session_id;
    uint32_t          user_id;
    pthread_mutex_t   lock;
    hash_table_t *    account_table_p;
    hash_table_t *    session_table_p;
    hash_table_t *    data_table_p;
};

#endif /* _CLIENT_ARGS_H */

/*** end of file ***/
