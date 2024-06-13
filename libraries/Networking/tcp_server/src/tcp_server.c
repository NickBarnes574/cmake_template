#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <poll.h> // poll(), EINTR
#include <stdio.h>
#include <stdlib.h>

#include "event_handler.h" // handle_connections()
#include "number_converter.h"
#include "server_structs.h"
#include "signal_handler.h" // CONINUE_RUNNING, SHUTDOWN, signal_action_setup(), check_for_signals()
#include "socket_io.h"      // create_socket()
#include "socket_manager.h" // sock_fd_arr_init(), close_all_sockets()
#include "tcp_server.h"
#include "utilities.h"

#define MAX_CLIENT_ADDRESS_SIZE 100 // Size for storing client address strings
#define MAX_TIMEOUT             300 // Maximum timeout in seconds (5 minutes)
#define MIN_TIMEOUT             10  // Minimum timeout in seconds
#define MAX_BACKLOG             128
#define MIN_BACKLOG             1
#define MIN_NUM_THREADS         2
#define MAX_NUM_THREADS         24
#define MIN_CLIENTS             1
#define MAX_CLIENTS             100

static int initialize_server(server_context_t * server);
static int run_server_loop(server_context_t * server);
static int validate_config(server_config_t * config);

int start_tcp_server(server_config_t * config)
{
    int              exit_code   = E_FAILURE;
    server_context_t server      = { 0 };
    socket_manager_t sock_mgr    = { 0 };
    threadpool_t *   thread_pool = NULL;

    if (NULL == config)
    {
        print_error("start_tcp_server(): NULL argument passed.");
        goto END;
    }

    exit_code = validate_config(config);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Error validating server config.");
        goto END;
    }

    thread_pool = threadpool_create(config->num_threads);
    if (NULL == thread_pool)
    {
        print_error("start_tcp_server(): Unable to create thread pool.");
        goto END;
    }

    server.sock_mgr    = &sock_mgr;
    server.thread_pool = thread_pool;
    server.config      = config;

    exit_code = initialize_server(&server);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Unable to initialize server.");
        goto END;
    }

    exit_code = run_server_loop(&server);
    if (E_SUCCESS != exit_code)
    {
        print_error("start_tcp_server(): Error running server loop.");
        goto END;
    }

END:
    close_all_sockets(server.sock_mgr);
    threadpool_destroy(&server.thread_pool);
    return exit_code;
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

    printf("----run_server_loop() - entered function\n");

    while (CONTINUE_RUNNING == signal)
    {
        printf("----run_server_loop() - top of while loop\n");
        signal = check_for_signals();
        if (SHUTDOWN == signal)
        {
            printf("Shutdown signal received.\n");
            goto END;
        }

        printf("----run_server_loop() - checked the signals\n");

        print_fd_array(server->sock_mgr);

        poll_count = poll(server->sock_mgr->fd_arr,
                          server->sock_mgr->fd_count,
                          server->config->timeout);

        printf("----run_server_loop() - poll count = %d\n", poll_count);

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

        if (0 == poll_count)
        {
            // Timeout occurred, no file descriptors were ready
            printf("----run_server_loop() - poll timeout occurred\n");
            continue;
        }

        exit_code = handle_connections(server);
        if (E_SUCCESS != exit_code)
        {
            print_error("run_server_loop(): handle_connections() failed.");
            goto END;
        }
        printf("----run_server_loop() - bottom of while loop\n");
    }

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

    printf("Port: '%s'\n",
           server->config->port); // Ensure that the port is correct

    exit_code = getaddrinfo(NULL, server->config->port, &hints, &addr_list);
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

    exit_code = set_fd_non_blocking(server->fd);
    if (E_SUCCESS != exit_code)
    {
        print_error("register_client(): Unable to set fd to non-blocking.");
        close(server->fd);
        goto END;
    }

    errno     = 0;
    exit_code = listen(server->fd, server->config->backlog_size);
    if (E_SUCCESS != exit_code)
    {
        perror("initialize_server(): listen() failed.");
        goto END;
    }

    exit_code = sock_mgr_init(server->sock_mgr,
                              server->fd,
                              server->config->max_clients,
                              DEFAULT_FD_CAPACITY);
    if (E_SUCCESS != exit_code)
    {
        print_error("initialize_server(): Unable to initialize sock manager.");
        goto END;
    }

END:
    freeaddrinfo(addr_list);
    return exit_code;
}

static int validate_config(server_config_t * config)
{
    int     exit_code = E_FAILURE;
    int32_t port      = 0;

    if (NULL == config)
    {
        print_error("validate_config(): NULL argument passed.");
        goto END;
    }

    exit_code = str_to_int32(config->port, &port);
    if (E_SUCCESS != exit_code)
    {
        print_error("validate_config(): Unable to convert port to integer.");
        goto END;
    }
    exit_code = E_FAILURE;

    // Check for well-known ports
    if ((0 <= port) && (1023 >= port))
    {
        print_error("validate_config(): Well-known port detected.");
        goto END;
    }

    if ((MIN_BACKLOG > config->backlog_size) ||
        (MAX_BACKLOG < config->backlog_size))
    {
        print_error("validate_config(): Invalid backlog size.");
        goto END;
    }

    if ((-1 != config->timeout) &&
        ((MIN_TIMEOUT > config->timeout) || (MAX_TIMEOUT < config->timeout)))
    {
        print_error("validate_config(): Invalid server timeout.");
        goto END;
    }

    if ((MIN_NUM_THREADS > config->num_threads) ||
        (MAX_NUM_THREADS < config->num_threads))
    {
        print_error("validate_config(): Invalid number of threads.");
        goto END;
    }

    if ((MIN_CLIENTS > config->max_clients) ||
        (MAX_CLIENTS < config->max_clients))
    {
        print_error("validate_config(): Invalid number of maximum clients.");
        goto END;
    }

    if (NULL == config->client_request)
    {
        print_error("validate_config(): NULL client request function.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}
