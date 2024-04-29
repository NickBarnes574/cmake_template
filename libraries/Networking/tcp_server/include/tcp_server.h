/**
 * @file tcp_server.h
 *
 * @brief
 */
#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include "threadpool.h"

#define SIG_SHUTDOWN 1

typedef struct config config_t;

typedef void * (*client_request_handler_t)(void *);
typedef void (*client_data_free_func_t)(void *);

typedef struct tcp_server_job
{
    void * (*client_function)(void *);
    void (*free_function)(void *);
    void * args_p;
} tcp_server_job_t;

typedef struct server
{
    int                      listening_socket;
    threadpool_t *           threadpool_p;
    client_request_handler_t client_request_handler;
    client_data_free_func_t  client_data_free_func;
    config_t *               settings_p;
} server_t;

typedef struct client_data
{
    int    client_fd;
    void * user_data_p;
} client_data_t;

/**
 * @brief Starts the TCP server and handles incoming client connections.
 *
 * This function initializes the server using provided parameters, and then
 * enters a loop to accept and handle incoming client connections. Each new
 * connection is processed in a separate job in the thread pool.
 *
 * @param port_p Pointer to a string representing the port number on which the
 * server will listen for incoming connections.
 * @param max_connections The maximum number of concurrent connections the
 * server should handle.
 * @param client_request_handler Function pointer for handling client requests.
 * @param client_data_free_func Function pointer for freeing client data.
 * @param user_data_p Pointer to user-defined data that can be passed to each
 * client handler.
 *
 * @return Returns E_SUCCESS on successful execution and E_FAILURE on error.
 *
 * @note The function uses a continuous loop to monitor incoming connections and
 * can be exited on SIGINT or SIGUSR1.
 */
int start_tcp_server(char *                   port_p,
                     size_t                   max_connections,
                     client_request_handler_t client_request_handler,
                     client_data_free_func_t  client_data_free_func,
                     void *                   user_data_p);

#endif /* _TCP_SERVER_NEW_H */

/*** end of file ***/
