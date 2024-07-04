/**
 * @file server_structs.h
 *
 * @brief Contains structures for managing server context, socket management,
 * and client context.
 */
#ifndef _SERVER_STRUCTS_H
#define _SERVER_STRUCTS_H

#include <netdb.h> // sockaddr_storage, socklen_t, INET6_ADDRSTRLEN

#include "tcp_server.h"
#include "threadpool.h" // threadpool_t

#define DEFAULT_FD_CAPACITY 5

/**
 * @struct socket_manager
 * @brief Manages an array of pollfd structures for handling multiple socket
 * descriptors.
 */
typedef struct socket_manager
{
    struct pollfd * fd_arr;      // Pointer to an array of pollfd structs.
    int             max_fds;     // Max number of file descriptors (clients).
    int             fd_count;    // Current number of managed file descriptors.
    int             fd_capacity; // Capacity of the array.
    pthread_mutex_t fd_mutex;    // Mutex for the client fd.
} socket_manager_t;

/**
 * @struct server_context
 * @brief Encapsulates all essential details and operational mechanisms of the
 * server.
 */
typedef struct server_context
{
    int                fd;          // File descriptor for the server socket.
    socket_manager_t * sock_mgr;    // Responsible for managing connections.
    threadpool_t *     thread_pool; // Responsible for processing requests.
    server_config_t *  config;      // Settings provided by the user.
} server_context_t;

/**
 * @struct client_context
 * @brief Stores context information for an individual client.
 */
typedef struct client_context
{
    int                     fd;       // File descriptor for the client socket.
    struct sockaddr_storage addr;     // Stores the client's socket address.
    socklen_t               addr_len; // Length of the socket address.
    char IP_addr[INET6_ADDRSTRLEN];   // String representaion of the client's IP
                                      // address.
} client_context_t;

#endif /* _SERVER_STRUCTS_H */

/*** end of file ***/
