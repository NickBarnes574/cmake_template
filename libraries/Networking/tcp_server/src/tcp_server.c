#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "signal_handler.h"
#include "socket_io.h"
#include "tcp_server.h"
#include "utilities.h"

#define MAX_CLIENT_ADDRESS_SIZE 100 // Size for storing client address strings
#define BACKLOG_SIZE            10 // Maximum number of pending client connections
#define DEFAULT_FD_CAPACITY     5
#define TIMEOUT                 (-1)
#define POLL_ERROR_EVENTS       (POLLERR | POLLHUP | POLLNVAL)
#define CLIENT_BUFFER_SIZE      256

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

static int create_socket(int * fd, struct addrinfo * addr);
static int initialize_server(server_context_t * server);
static int setup_poll(socket_manager_t * context, int fd);
static int run_server_loop(server_context_t * server);
static int handle_connections(server_context_t * server);
static int register_client(server_context_t * server);
static int fd_add(socket_manager_t * sock_mgr, int new_fd);
static int fd_remove(socket_manager_t * sock_mgr, int index);
static int increase_capacity(socket_manager_t * sock_mgr);
static int handle_client_activity(socket_manager_t * sock_mgr, int index);
static int broadcast_data_to_clients(socket_manager_t * sock_mgr,
                                     int                client_fd,
                                     char *             buffer);
static int receive_data_from_client(int                client_fd,
                                    char *             buffer,
                                    socket_manager_t * sock_mgr,
                                    int                index);

int start_tcp_server(char * port)
{
    int              exit_code = E_FAILURE;
    server_context_t server    = { 0 };
    socket_manager_t sock_mgr  = { 0 };

    if (NULL == port)
    {
        print_error("start_tcp_server(): NULL argument passed.");
        goto END;
    }

    server.port      = port;
    sock_mgr.max_fds = DEFAULT_FD_CAPACITY;

    exit_code = initialize_server(&server);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Unable to initialize server.");
        goto END;
    }

    exit_code = setup_poll(&sock_mgr, server.fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Unable to setup poll.");
        goto END;
    }

    server.sock_mgr = &sock_mgr;

    exit_code = run_server_loop(&server);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Error running server loop.");
        goto END;
    }

END:
    if (server.fd)
    {
        close(server.fd);
    }
    free(sock_mgr.fd_arr);
    sock_mgr.fd_arr = NULL;
    return exit_code;
}

void * get_in_addr(struct sockaddr * sa_p)
{
    void * address = NULL;

    if (sa_p == NULL)
    {
        goto END;
    }

    if (sa_p->sa_family == AF_INET)
    {
        struct sockaddr_in * ipv4_addr = (struct sockaddr_in *)sa_p;
        address                        = &(ipv4_addr->sin_addr);
    }
    else
    {
        struct sockaddr_in6 * ipv6_addr = (struct sockaddr_in6 *)sa_p;
        address                         = &(ipv6_addr->sin6_addr);
    }

END:
    return address;
}

static int increase_capacity(socket_manager_t * sock_mgr)
{
    int             exit_code    = E_FAILURE;
    int             new_capacity = 0;
    struct pollfd * new_array    = NULL;

    if (NULL == sock_mgr)
    {
        print_error("increase_capacity(): NULL argument passed.");
        goto END;
    }

    new_capacity = sock_mgr->fd_capacity * 2;
    new_array =
        realloc(sock_mgr->fd_arr, sizeof(*sock_mgr->fd_arr) * new_capacity);
    if (NULL == new_array)
    {
        print_error("increase_capacity(): CMR failure - 'fd_arr'.");
        goto END;
    }

    sock_mgr->fd_arr      = new_array;
    sock_mgr->fd_capacity = new_capacity;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int fd_add(socket_manager_t * sock_mgr, int new_fd)
{
    int exit_code = E_FAILURE;

    if ((NULL == sock_mgr))
    {
        print_error("fd_add(): NULL argument passed.");
        goto END;
    }

    if (sock_mgr->fd_count == sock_mgr->fd_capacity)
    {
        exit_code = increase_capacity(sock_mgr);
        if (E_SUCCESS != exit_code)
        {
            print_error("fd_add(): Unable to check capacity.");
            goto END;
        }
    }

    // Add new client fd to the array
    sock_mgr->fd_arr[sock_mgr->fd_count].fd     = new_fd;
    sock_mgr->fd_arr[sock_mgr->fd_count].events = POLLIN;
    sock_mgr->fd_count++;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int fd_remove(socket_manager_t * sock_mgr, int index)
{
    int exit_code = E_FAILURE;

    if (NULL == sock_mgr)
    {
        print_error("fd_remove(): NULL argument passed.");
        goto END;
    }

    sock_mgr->fd_arr[index] = sock_mgr->fd_arr[sock_mgr->fd_count - 1];
    sock_mgr->fd_count--;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int register_client(server_context_t * server)
{
    int              exit_code = E_FAILURE;
    client_context_t client    = { 0 };

    if (NULL == server)
    {
        print_error("register_client(): NULL argument passed.");
        goto END;
    }

    client.addr_len = sizeof(client.addr);

    errno = 0;
    client.fd =
        accept(server->fd, (struct sockaddr *)&client.addr, &client.addr_len);
    if (-1 == client.fd)
    {
        perror("register_client(): accept() failed.");
        goto END;
    }

    exit_code = fd_add(server->sock_mgr, client.fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("register_client(): Unable to add fd to array.");
        goto END;
    }

    printf("pollserver: new connection from %s on socket %d\n",
           inet_ntop(client.addr.ss_family,
                     get_in_addr((struct sockaddr *)&client.addr),
                     client.IP_addr,
                     INET6_ADDRSTRLEN),
           client.fd);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int receive_data_from_client(int                client_fd,
                                    char *             buffer,
                                    socket_manager_t * sock_mgr,
                                    int                index)
{
    int exit_code = E_FAILURE;

    if ((NULL == buffer) || (NULL == sock_mgr))
    {
        print_error("receive_data_from_client(): NULL argument passed.");
        goto END;
    }

    exit_code = recv_data(client_fd, &buffer);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_activity(): recv_data() failed.");
        close(client_fd);
        exit_code = fd_remove(sock_mgr, index);
        if (E_SUCCESS != exit_code)
        {
            print_error("handle_client_activity(): Unable to remove fd.");
        }
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int broadcast_data_to_clients(socket_manager_t * sock_mgr,
                                     int                client_fd,
                                     char *             buffer)
{
    int exit_code      = E_FAILURE;
    int destination_fd = 0;

    for (int idx_b = 0; idx_b < sock_mgr->fd_count; idx_b++)
    {
        destination_fd = sock_mgr->fd_arr[idx_b].fd;
        if ((destination_fd != client_fd) &&
            (destination_fd != sock_mgr->fd_arr[0].fd))
        {
            exit_code = send_data(destination_fd, &buffer);
            if (E_SUCCESS != exit_code)
            {
                print_error("handle_client_activity(): send_data() failed.");
                goto END;
            }
        }
    }

END:
    return exit_code;
}

static int handle_client_activity(socket_manager_t * sock_mgr, int index)
{
    int  exit_code                  = E_FAILURE;
    char buffer[CLIENT_BUFFER_SIZE] = { 0 };
    int  client_fd                  = 0;

    if ((NULL == sock_mgr) || (NULL == sock_mgr->fd_arr))
    {
        print_error("handle_client_activity(): NULL argument passed.");
        goto END;
    }

    client_fd = sock_mgr->fd_arr[index].fd;

    exit_code = receive_data_from_client(client_fd, &buffer, sock_mgr, index);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_activity(): Unable to receive data.");
        goto END;
    }

    exit_code = broadcast_data_to_clients(sock_mgr, client_fd, &buffer);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_activity(): Unable to broadcast data.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int handle_connections(server_context_t * server)
{
    int             exit_code = E_FAILURE;
    struct pollfd * fd_entry  = NULL;

    if ((NULL == server) || (NULL == server->sock_mgr))
    {
        print_error("handle_connections(): NULL argument passed.");
        return E_FAILURE;
    }

    for (int idx = 0; idx < server->sock_mgr->fd_count; idx++)
    {
        fd_entry = &server->sock_mgr->fd_arr[idx];

        if (fd_entry->revents & POLL_ERROR_EVENTS)
        {
            print_error("handle_connections(): Error on socket.");
            close(fd_entry->fd);
            fd_entry->fd = -1;
            continue; // Skip if we get a socket error
        }

        if (0 == (fd_entry->revents & POLLIN))
        {
            continue; // Skip if there's no data to read
        }

        if (fd_entry->fd == server->fd)
        {
            exit_code = register_client(server);
            if (E_SUCCESS != exit_code)
            {
                // Continue processing even when a new connection fails
                continue;
            }
        }
        else
        {
            exit_code = handle_client_activity(server->sock_mgr, idx);
            if (E_SUCCESS != exit_code)
            {
                continue;
            }
        }
    }

    return E_SUCCESS;
}

static int run_server_loop(server_context_t * server)
{
    int exit_code  = E_FAILURE;
    int signal     = CONTINUE_RUNNING;
    int poll_count = 0;

    if ((NULL == server) || (NULL == server->sock_mgr))
    {
        print_error("run_server_loop(): NULL argument passed.");
        goto END;
    }

    server->sock_mgr->fd_count    = 1; // For the server fd
    server->sock_mgr->fd_capacity = DEFAULT_FD_CAPACITY;

    while (CONTINUE_RUNNING == signal)
    {
        signal = check_for_signals();
        if (SHUTDOWN == signal)
        {
            printf("Shutdown signal received.\n");
            goto END;
        }

        poll_count =
            poll(server->sock_mgr->fd_arr, server->sock_mgr->fd_count, TIMEOUT);
        if (0 > poll_count)
        {
            if (errno == EINTR)
            {
                // If interrupted by a signal, check the signal and continue the
                // loop
                continue;
            }
            else
            {
                perror("run_server_loop(): poll() failed.");
                goto END;
            }
        }

        exit_code = handle_connections(server);
        if (E_SUCCESS != exit_code)
        {
            print_error("run_server_loop(): handle_connections() failed.");
            goto END;
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int setup_poll(socket_manager_t * sock_mgr, int fd)
{
    int exit_code = E_FAILURE;

    if (NULL == sock_mgr)
    {
        print_error("setup_poll(): NULL argument passed.");
        goto END;
    }

    sock_mgr->fd_arr = calloc(DEFAULT_FD_CAPACITY, sizeof(struct pollfd));
    if (NULL == sock_mgr->fd_arr)
    {
        print_error("setup_poll(): CMR failure - 'fd_arr'.");
        goto END;
    }

    if (0 >= sock_mgr->max_fds)
    {
        print_error("setup_poll(): 'max_fds' must be greater than 0");
        goto END;
    }

    sock_mgr->fd_arr[0].fd     = fd;     // Set up the server socket
    sock_mgr->fd_arr[0].events = POLLIN; // Check for read events
    sock_mgr->fd_count         = 1;

    // Initialize the rest of the array
    for (int idx = 1; idx < sock_mgr->max_fds; idx++)
    {
        sock_mgr->fd_arr[idx].fd     = -1; // Unused slots are set to `-1`
        sock_mgr->fd_arr[idx].events = 0;
    }

    exit_code = E_SUCCESS;
END:
    if (E_SUCCESS != exit_code)
    {
        free(sock_mgr->fd_arr);
        sock_mgr->fd_arr = NULL;
    }
    return exit_code;
}

static int create_socket(int * fd, struct addrinfo * addr)
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

    *fd = sock_fd;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int initialize_server(server_context_t * server)
{
    int               exit_code = E_FAILURE;
    struct addrinfo   hints     = { 0 };
    struct addrinfo * addr_list = NULL;
    struct addrinfo * current   = NULL;

    if (NULL == server)
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

    printf("Port: '%s'\n", server->port); // Ensure that the port is correct

    exit_code = getaddrinfo(NULL, server->port, &hints, &addr_list);
    if (E_SUCCESS != exit_code)
    {
        fprintf(stderr,
                "initialize_server(): getaddrinfo failed with error: %s\n",
                gai_strerror(exit_code));
        goto END;
    }

    // Try each address, attempting to create a socket and bind
    for (current = addr_list; NULL != current; current = current->ai_next)
    {
        exit_code = create_socket(&server->fd, current);
        if (E_SUCCESS != exit_code)
        {
            continue;
        }

        exit_code = bind(server->fd, current->ai_addr, current->ai_addrlen);
        if (E_SUCCESS == exit_code)
        {
            break; // Successfully bound socket
        }

        close(server->fd);
    }

    if (NULL == current)
    {
        // None of the addresses succeeded
        exit_code = E_FAILURE;
        goto END;
    }

    errno     = 0;
    exit_code = listen(server->fd, BACKLOG_SIZE);
    if (E_SUCCESS != exit_code)
    {
        perror("initialize_server(): listen() failed.");
        goto END;
    }

END:
    freeaddrinfo(addr_list);
    return exit_code;
}