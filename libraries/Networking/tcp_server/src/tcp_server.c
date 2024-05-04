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
#define DEFAULT_FD_CAPACITY     5
#define TIMEOUT                 (-1)

static int create_socket(int * server_fd, struct addrinfo * addr);
static int initialize_server(int * server_fd, const char * port);
static int setup_poll(struct pollfd * fd_array, int max_fds, int server_fd);
static int run_server_loop(struct pollfd * fd_array,
                           int             max_fds,
                           int             server_fd);
static int check_existing_connections(struct pollfd * fd_array,
                                      int *           fd_count,
                                      int *           fd_capacity,
                                      int             server_fd);
static int handle_new_connection(struct pollfd * fd_array,
                                 int *           fd_count,
                                 int *           fd_capacity,
                                 int             server_fd,
                                 int *           new_client_fd);
static int add_to_fd_array(struct pollfd ** fd_array,
                           int              new_fd,
                           int *            fd_count,
                           int *            fd_capacity);

static int add_to_fd_array(struct pollfd ** fd_array,
                           int              new_fd,
                           int *            fd_count,
                           int *            fd_capacity)
{
    int exit_code = E_FAILURE;

    if ((NULL == fd_array) || (NULL == *fd_array) || (NULL == fd_count) ||
        (NULL == fd_capacity))
    {
        print_error("add_to_fd_array(): NULL argument passed.");
        goto END;
    }

    if (*fd_count == *fd_capacity)
    {
        *fd_capacity = *fd_capacity * 2; // Double the capacity
        *fd_array    = realloc(*fd_array, sizeof(**fd_array) * (*fd_capacity));
        if (NULL == *fd_array)
        {
            print_error("add_to_fd_array(): CMR failure - 'fd_array'.");
            goto END;
        }
    }

    (*fd_array)[*fd_count].fd     = new_fd;
    (*fd_array)[*fd_count].events = POLLIN;

    (*fd_count)++;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int handle_new_connection(struct pollfd * fd_array,
                                 int *           fd_count,
                                 int *           fd_capacity,
                                 int             server_fd,
                                 int *           new_client_fd)
{
    int                     exit_code   = E_FAILURE;
    int                     new_fd      = 0;
    struct sockaddr_storage client_addr = { 0 };
    socklen_t               addr_length = 0;

    if ((NULL == fd_array) || (NULL == new_client_fd))
    {
        print_error("handle_new_connection(): NULL argument passed.");
        goto END;
    }

    addr_length = sizeof(client_addr);

    errno  = 0;
    new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_length);
    if (-1 == new_fd)
    {
        perror("handle_new_connection(): accept() failed.");
        goto END;
    }

    exit_code = add_to_fd_array(&fd_array, new_fd, fd_count, fd_capacity);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_new_connection(): Unable to add fd to array.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int check_existing_connections(struct pollfd * fd_array,
                                      int *           fd_count,
                                      int *           fd_capacity,
                                      int             server_fd)
{
    int             exit_code = E_FAILURE;
    int             new_fd    = 0;
    struct pollfd * fd_entry  = NULL;

    if (NULL == fd_array)
    {
        print_error("check_existing_connections(): NULL argument passed.");
        goto END;
    }

    for (int idx = 0; idx < *fd_count; idx++)
    {
        fd_entry = &fd_array[idx];
        if (fd_array[idx].revents & (POLLERR | POLLHUP | POLLNVAL))
        {
            print_error("check_existing_connections(): Error on socket.");
            continue;
            // NOTE: Possibly close fd and remove it from the array
        }

        if (NULL == (fd_entry->revents & POLLIN))
        {
            continue; // Skip to the next iteration if there's no data to read
        }

        if (fd_entry == server_fd)
        {
            exit_code = handle_new_connection(
                fd_array, fd_count, fd_capacity, server_fd, &new_fd);
            if (E_SUCCESS != exit_code)
            {
                print_error(
                    "check_existing_connections(): Error handling new "
                    "connection.");
                goto END;
            }
        }
        else
        {
            // Handle client activity
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int run_server_loop(struct pollfd * fd_array, int max_fds, int server_fd)
{
    int exit_code   = E_FAILURE;
    int fd_count    = 1; // For the server fd
    int fd_capacity = DEFAULT_FD_CAPACITY;
    int poll_count  = 0;

    if (NULL == fd_array)
    {
        print_error("run_server_loop(): NULL argument passed.");
        goto END;
    }

    for (;;)
    {
        poll_count = poll(fd_array, fd_count, TIMEOUT);
        if (-1 == poll_count)
        {
            perror("run_server_loop(): poll() failed.");
            goto END;
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

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

    fd_array = calloc(DEFAULT_FD_CAPACITY, sizeof(struct pollfd));
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

    exit_code = setup_poll(fd_array, DEFAULT_FD_CAPACITY, server_fd);
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