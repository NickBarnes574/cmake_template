#include <errno.h>
#include <poll.h> // poll()
#include <stdio.h>
#include <stdlib.h>

#include "connection_manager.h"
#include "server_ops.h"
#include "signal_handler.h"
#include "utilities.h"

#define BACKLOG_SIZE 10 // Maximum number of pending client connections
#define TIMEOUT      (-1)

int run_server_loop(server_context_t * server)
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

int create_socket(int * fd, struct addrinfo * addr)
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

int initialize_server(server_context_t * server)
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

/*** end of file ***/
