#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "socket_io.h"
#include "utilities.h"

#define MAX_CLIENT_ADDRESS_SIZE 100 // Size for storing client address strings
#define BACKLOG_SIZE            10 // Maximum number of pending client connections
#define DEFAULT_FD_CAPACITY     5
#define TIMEOUT                 (-1)
#define POLL_ERROR_EVENTS       (POLLERR | POLLHUP | POLLNVAL)
#define CLIENT_BUFFER_SIZE      256

typedef struct server_config
{
    int    server_fd;
    char * port;
} server_config_t;

typedef struct poll_context
{
    struct pollfd * fd_array;
    int             max_fds;
    int             fd_count;
    int             fd_capacity;
} poll_context_t;

typedef struct connection_context
{
    poll_context_t * poll_ctx;
    int              server_fd;
    int              client_fd;
} connection_context_t;

static int create_socket(int * server_fd, struct addrinfo * addr);
static int initialize_server(server_config_t * config);
static int setup_poll(poll_context_t * context, int server_fd);
static int run_server_loop(poll_context_t * poll_ctx, int server_fd);
static int check_connections(poll_context_t * poll_ctx, int server_fd);
static int new_connection(connection_context_t * conn_ctx);
static int fd_array_add(poll_context_t * poll_ctx, int new_fd);
static int fd_array_remove(poll_context_t * poll_ctx, int index);
static int check_capacity(poll_context_t * poll_ctx);
static int handle_client_activity(connection_context_t * conn_ctx, int index);
void *     get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
static int check_capacity(poll_context_t * poll_ctx)
{
    int             exit_code    = E_FAILURE;
    int             new_capacity = 0;
    struct pollfd * new_array    = NULL;

    if (NULL == poll_ctx)
    {
        print_error("check_capacity(): NULL argument passed.");
        goto END;
    }

    new_capacity = poll_ctx->fd_capacity * 2;
    new_array =
        realloc(poll_ctx->fd_array, sizeof(*poll_ctx->fd_array) * new_capacity);
    if (NULL == new_array)
    {
        print_error("check_capacity(): CMR failure - 'fd_array'.");
        goto END;
    }

    poll_ctx->fd_array    = new_array;
    poll_ctx->fd_capacity = new_capacity;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int fd_array_add(poll_context_t * poll_ctx, int new_fd)
{
    int exit_code = E_FAILURE;

    if ((NULL == poll_ctx))
    {
        print_error("fd_array_add(): NULL argument passed.");
        goto END;
    }

    exit_code = check_capacity(poll_ctx);
    if (E_SUCCESS != exit_code)
    {
        print_error("fd_array_add(): Unable to check capacity.");
        goto END;
    }

    // Add new client fd to the array
    poll_ctx->fd_array[poll_ctx->fd_count].fd     = new_fd;
    poll_ctx->fd_array[poll_ctx->fd_count].events = POLLIN;
    poll_ctx->fd_count++;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int fd_array_remove(poll_context_t * poll_ctx, int index)
{
    int exit_code = E_FAILURE;

    if (NULL == poll_ctx)
    {
        print_error("fd_array_remove(): NULL argument passed.");
        goto END;
    }

    poll_ctx->fd_array[index] = poll_ctx->fd_array[poll_ctx->fd_count - 1];
    poll_ctx->fd_count--;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int new_connection(connection_context_t * conn_ctx)
{
    int                     exit_code   = E_FAILURE;
    int                     new_fd      = 0;
    struct sockaddr_storage client_addr = { 0 };
    socklen_t               addr_length = 0;
    char                    clientIP[INET6_ADDRSTRLEN];

    if (NULL == conn_ctx)
    {
        print_error("new_connection(): NULL argument passed.");
        goto END;
    }

    addr_length = sizeof(client_addr);

    errno  = 0;
    new_fd = accept(
        conn_ctx->server_fd, (struct sockaddr *)&client_addr, &addr_length);
    if (-1 == new_fd)
    {
        perror("new_connection(): accept() failed.");
        goto END;
    }

    exit_code = fd_array_add(conn_ctx->poll_ctx, new_fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("new_connection(): Unable to add fd to array.");
        goto END;
    }

    printf("pollserver: new connection from %s on socket %d\n",
           inet_ntop(client_addr.ss_family,
                     get_in_addr((struct sockaddr *)&client_addr),
                     clientIP,
                     INET6_ADDRSTRLEN),
           new_fd);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int handle_client_activity(connection_context_t * conn_ctx, int index)
{
    int  exit_code                  = E_FAILURE;
    char buffer[CLIENT_BUFFER_SIZE] = { 0 };
    int  destination_fd             = 0;

    if ((NULL == conn_ctx) || (NULL == conn_ctx->poll_ctx) ||
        (NULL == conn_ctx->poll_ctx->fd_array))
    {
        print_error("handle_client_activity(): NULL argument passed.");
        goto END;
    }

    conn_ctx->client_fd = conn_ctx->poll_ctx->fd_array[index].fd;

    exit_code = recv_data(conn_ctx->client_fd, &buffer);
    if (E_SUCCESS != exit_code)
    {
        print_error("handle_client_activity(): recv_data() failed.");
        close(conn_ctx->client_fd);
        exit_code = fd_array_remove(conn_ctx->poll_ctx, index);
        if (E_SUCCESS != exit_code)
        {
            print_error("handle_client_activity(): Unable to remove fd.");
        }
        goto END;
    }

    for (int idx_b = 0; idx_b < conn_ctx->poll_ctx->fd_count; idx_b++)
    {
        destination_fd = conn_ctx->poll_ctx->fd_array[idx_b].fd;
        if ((destination_fd != conn_ctx->server_fd) &&
            (destination_fd != conn_ctx->client_fd))
        {
            exit_code = send_data(destination_fd, &buffer);
            if (E_SUCCESS != exit_code)
            {
                print_error("handle_client_activity(): send_data() failed.");
                goto END;
            }
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int check_connections(poll_context_t * poll_ctx, int server_fd)
{
    int                  exit_code = E_FAILURE;
    struct pollfd *      fd_entry  = NULL;
    connection_context_t conn_ctx  = { 0 };

    if (NULL == poll_ctx)
    {
        print_error("check_connections(): NULL argument passed.");
        goto END;
    }

    for (int idx = 0; idx < poll_ctx->fd_count; idx++)
    {
        fd_entry = &poll_ctx->fd_array[idx];

        if (0 != (fd_entry->revents & POLL_ERROR_EVENTS))
        {
            print_error("check_connections(): Error on socket.");
            close(fd_entry->fd);
            fd_entry->fd = -1;
            continue; // Skip if we get a socket error
        }

        if (0 == (fd_entry->revents & POLLIN))
        {
            continue; // Skip if there's no data to read
        }

        if (fd_entry->fd == server_fd)
        {
            conn_ctx.poll_ctx  = poll_ctx;
            conn_ctx.server_fd = server_fd;
            conn_ctx.client_fd = -1;

            exit_code = new_connection(&conn_ctx);
            if (E_SUCCESS != exit_code)
            {
                print_error("check_connections(): New connection error.");
                continue; // Continue processing even a connection fails
            }
        }
        else
        {
            conn_ctx.poll_ctx  = poll_ctx;
            conn_ctx.server_fd = server_fd;

            exit_code = handle_client_activity(&conn_ctx, idx);
            if (E_SUCCESS != exit_code)
            {
                print_error("check_connections(): Error handling client.");
                continue; // Continue processing even if a connection fails
            }
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int run_server_loop(poll_context_t * poll_ctx, int server_fd)
{
    int exit_code  = E_FAILURE;
    int poll_count = 0;

    if (NULL == poll_ctx)
    {
        print_error("run_server_loop(): NULL argument passed.");
        goto END;
    }

    poll_ctx->fd_count    = 1; // For the server fd
    poll_ctx->fd_capacity = DEFAULT_FD_CAPACITY;

    for (;;)
    {
        poll_count = poll(poll_ctx->fd_array, poll_ctx->fd_count, TIMEOUT);
        if (-1 == poll_count)
        {
            perror("run_server_loop(): poll() failed.");
            goto END;
        }

        exit_code = check_connections(poll_ctx, server_fd);
        if (E_SUCCESS != exit_code)
        {
            print_error("run_server_loop(): check_connections() failed.");
            goto END;
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int start_tcp_server(char * port)
{
    int             exit_code = E_FAILURE;
    server_config_t config    = { 0 };
    poll_context_t  poll_ctx  = { 0 };

    if (NULL == port)
    {
        print_error("start_tcp_server(): NULL argument passed.");
        goto END;
    }

    config.port      = port;
    poll_ctx.max_fds = DEFAULT_FD_CAPACITY;

    exit_code = initialize_server(&config);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Unable to initialize server.");
        goto END;
    }

    exit_code = setup_poll(&poll_ctx, config.server_fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Unable to setup poll.");
        goto END;
    }

    exit_code = run_server_loop(&poll_ctx, config.server_fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Error running server loop.");
        goto END;
    }

    exit_code = E_SUCCESS;

END:
    return exit_code;
}

static int setup_poll(poll_context_t * poll_ctx, int server_fd)
{
    int exit_code = E_FAILURE;

    if (NULL == poll_ctx)
    {
        print_error("setup_poll(): NULL argument passed.");
        goto END;
    }

    poll_ctx->fd_array = calloc(DEFAULT_FD_CAPACITY, sizeof(struct pollfd));
    if (NULL == poll_ctx->fd_array)
    {
        print_error("setup_poll(): CMR failure - 'fd_array'.");
        goto END;
    }

    if (0 >= poll_ctx->max_fds)
    {
        print_error("setup_poll(): 'max_fds' must be greater than 0");
        goto END;
    }

    poll_ctx->fd_array[0].fd     = server_fd; // Set up the server socket
    poll_ctx->fd_array[0].events = POLLIN;    // Check for read events
    poll_ctx->fd_count           = 1;

    // Initialize the rest of the array
    for (int idx = 1; idx < poll_ctx->max_fds; idx++)
    {
        poll_ctx->fd_array[idx].fd     = -1; // Unused slots are set to `-1`
        poll_ctx->fd_array[idx].events = 0;
    }

    exit_code = E_SUCCESS;
END:
    if (E_SUCCESS != exit_code)
    {
        free(poll_ctx->fd_array);
        poll_ctx->fd_array = NULL;
    }
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

static int initialize_server(server_config_t * config)
{
    int               exit_code = E_FAILURE;
    struct addrinfo   hints     = { 0 };
    struct addrinfo * addr_list = NULL;
    struct addrinfo * current   = NULL;

    if (NULL == config)
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

    printf("Port: '%s'\n", config->port); // Ensure that the port is correct

    exit_code = getaddrinfo(NULL, config->port, &hints, &addr_list);
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
        exit_code = create_socket(&config->server_fd, current);
        if (E_SUCCESS != exit_code)
        {
            continue;
        }

        exit_code =
            bind(config->server_fd, current->ai_addr, current->ai_addrlen);
        if (E_SUCCESS == exit_code)
        {
            break; // Successfully bound socket
        }

        close(config->server_fd);
    }

    if (NULL == current)
    {
        // None of the addresses succeeded
        exit_code = E_FAILURE;
        goto END;
    }

    errno     = 0;
    exit_code = listen(config->server_fd, BACKLOG_SIZE);
    if (E_SUCCESS != exit_code)
    {
        perror("initialize_server(): listen() failed.");
        goto END;
    }

END:
    freeaddrinfo(addr_list);
    return exit_code;
}