#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

#include "threadpool.h"
#include "utilities.h"

#define MAX_CLIENT_ADDRESS_SIZE 100 // Size for storing client address strings
#define BACKLOG_SIZE            10 // Maximum number of pending client connections
#define MAX_FD_COUNT            50

static int create_socket(int * server_fd, struct addrinfo * addr);
static int initialize_server(int * server_fd, const char * port);
static int setup_poll(struct pollfd * fd_array, int max_fds, int server_fd);

int start_tcp_server(char * port, size_t max_connections)
{
    int             exit_code  = E_FAILURE;
    int             server_fd  = 0;
    struct pollfd * fd_array   = NULL;
    threadpool_t *  threadpool = NULL;

    if (NULL == port)
    {
        print_error("start_tcp_server(): NULL argument passed.");
        goto END;
    }

    fd_array = calloc(MAX_FD_COUNT, sizeof(struct pollfd));
    if (NULL == fd_array)
    {
        print_error("start_tcp_server(): CMR failure - 'fd_array'.");
        goto END;
    }

    exit_code = initialize_server(&server_fd, port);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Unable to initialize server.");
        goto END;
    }

    exit_code = setup_poll(fd_array, MAX_FD_COUNT, server_fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Unable to setup poll.");
        goto END;
    }

    threadpool = threadpool_create(max_connections);
    if (NULL == threadpool)
    {
        print_error("start_tcp_server(): Unable to create threadpool.");
        goto END;
    }

    // Implement main server loop and event handling here

    exit_code = E_SUCCESS;

END:
    threadpool_destroy(&threadpool);
    threadpool = NULL;
    free(fd_array);
    fd_array = NULL;
    return exit_code;
}

static int setup_poll(struct pollfd * fd_array, int max_fds, int server_fd)
{
    int exit_code = E_FAILURE;

    if (NULL == fd_array)
    {
        print_error("setup_poll(): NULL argument passed.");
        goto END;
    }

    if (0 >= max_fds)
    {
        print_error("setup_poll(): 'max_fds' must be greater than 0");
        goto END;
    }

    fd_array[0].fd     = server_fd; // Set up the first fd for the server socket
    fd_array[0].events = POLLIN;    // Check for read events

    // Initialize the rest of the array
    for (int idx = 1; idx < max_fds; idx++)
    {
        fd_array[idx].fd     = -1; // Unused slots are set to `-1`
        fd_array[idx].events = 0;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int create_socket(int * server_fd, struct addrinfo * addr)
{
    int exit_code = E_FAILURE;
    int optval    = 1; // Enable SO_REUSEADDR
    int sock_fd   = 0;

    if (NULL == addr)
    {
        print_error("create_socket(): NULL argument passed.");
        goto END;
    }

    errno   = 0;
    sock_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (-1 == sock_fd)
    {
        perror("create_socket(): socket() failed.");
        goto END;
    }

    // Enable the SO_REUSEADDR option to reuse the local address
    exit_code =
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (E_SUCCESS != exit_code)
    {
        perror("create_socket(): setsockopt() failed.");
        close(sock_fd);
        goto END;
    }

    *server_fd = sock_fd;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int initialize_server(int * server_fd, const char * port)
{
    int               exit_code = E_FAILURE;
    struct addrinfo   hints     = { 0 };
    struct addrinfo * addr_list = NULL;
    struct addrinfo * current   = NULL;

    if ((NULL == server_fd) || (NULL == port))
    {
        print_error("initialize_server(): NULL argument passed.");
        goto END;
    }

    // Initialize a TCP server address using IPV4
    hints.ai_family    = AF_INET;
    hints.ai_socktype  = SOCK_STREAM;
    hints.ai_flags     = AI_PASSIVE;
    hints.ai_protocol  = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;

    exit_code = getaddrinfo(NULL, port, &hints, &addr_list);
    if (E_SUCCESS != exit_code)
    {
        print_error("initialize_server(): getaddrinfo failed.");
        goto END;
    }

    // Try each address, attempting to create a socket and bind
    for (current = addr_list; NULL != current; current = current->ai_next)
    {
        exit_code = create_socket(server_fd, current);
        if (E_SUCCESS != exit_code)
        {
            continue;
        }

        exit_code = bind(*server_fd, current->ai_addr, current->ai_addrlen);
        if (E_SUCCESS == exit_code)
        {
            break; // Successfully bound socket
        }

        close(*server_fd);
    }

    if (NULL == current)
    {
        // None of the addresses succeeded
        exit_code = E_FAILURE;
        goto END;
    }

    errno     = 0;
    exit_code = listen(*server_fd, BACKLOG_SIZE);
    if (E_SUCCESS != exit_code)
    {
        perror("initialize_server(): listen() failed.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    freeaddrinfo(addr_list);
    return exit_code;
}