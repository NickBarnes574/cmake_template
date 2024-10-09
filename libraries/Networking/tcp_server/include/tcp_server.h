/**
 * @file tcp_server.h
 *
 * @brief
 */
#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

typedef void * (*request_func_t)(void *);

typedef struct server_config
{
    char *         port;         // The port on which the server is listening.
    int            max_clients;  // Max number of clients the server can handle.
    int            timeout;      // Poll timeout duration.
    int            num_threads;  // Number of threads in the thread pool.
    int            backlog_size; // Backlog size for listen().
    request_func_t client_request; // User-defined request handling function.
} server_config_t;

int start_tcp_server(server_config_t * config);

#endif /* _TCP_SERVER_H */

/*** end of file ***/
