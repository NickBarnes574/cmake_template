/**
 * @file server_structs.h
 *
 * @brief
 */
#ifndef _SERVER_STRUCTS_H
#define _SERVER_STRUCTS_H

#include <netdb.h> // sockaddr_storage, socklen_t, INET6_ADDRSTRLEN

#define DEFAULT_FD_CAPACITY 5

typedef struct socket_manager
{
    struct pollfd * fd_arr;
    int             max_fds;
    int             fd_count;
    int             fd_capacity;
} socket_manager_t;

typedef struct server_context
{
    int                fd;
    char *             port;
    socket_manager_t * sock_mgr;
} server_context_t;

typedef struct client_context
{
    int                     fd;
    struct sockaddr_storage addr;
    socklen_t               addr_len;
    char                    IP_addr[INET6_ADDRSTRLEN];
} client_context_t;

#endif /* _SERVER_STRUCTS_H */

/*** end of file ***/
