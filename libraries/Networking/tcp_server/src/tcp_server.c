#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>

#include "server_ops.h"
#include "server_structs.h"
#include "signal_handler.h"
#include "socket_manager.h"
#include "tcp_server.h"
#include "utilities.h"

#define MAX_CLIENT_ADDRESS_SIZE 100 // Size for storing client address strings
#define BACKLOG_SIZE            10 // Maximum number of pending client connections

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
