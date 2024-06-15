#include <stdio.h>
#include <stdlib.h>

#include "job_handler.h"
#include "signal_handler.h"
#include "tcp_server.h"
#include "utilities.h"

#define BACKLOG_SIZE 10   // Maximum number of pending client connections.
#define NUM_THREADS  10   // Number of threads to create.
#define MAX_CLIENTS  100  // Maximum number of clients the server can handle.
#define TIMEOUT      (10) // Poll timeout duration.

int main(void)
{
    int               exit_code = E_FAILURE;
    server_config_t * config    = NULL;

    // Initialize signal handler
    exit_code = signal_action_setup();
    if (E_SUCCESS != exit_code)
    {
        print_error("main(): Unable to setup signal handler.");
        goto END;
    }

    config = calloc(1, sizeof(server_config_t));
    if (NULL == config)
    {
        print_error("main(): CMR failure - config.");
        goto END;
    }

    config->port           = "17337";
    config->max_clients    = MAX_CLIENTS;
    config->num_threads    = NUM_THREADS;
    config->backlog_size   = BACKLOG_SIZE;
    config->timeout        = TIMEOUT;
    config->client_request = process_client_request;

    exit_code = start_tcp_server(config);
    if (E_SUCCESS != exit_code)
    {
        print_error("main(): start_tcp_server() failed.");
        goto END;
    }

END:
    free(config);
    config = NULL;
    return exit_code;
}
