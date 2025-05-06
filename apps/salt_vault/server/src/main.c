#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "option_handler.h"
#include "signal_handler.h"
#include "tcp_server.h"
#include "utilities.h"

#define MIN_ARGS             1       // Min number of CLI arguments
#define MAX_ARGS             6       // Max number of CLI arguments
#define DEFAULT_PORT_NUMBER  "31337" // Default port to use
#define DEFAULT_THREAD_COUNT 4       // Default number of threads to use
#define BACKLOG_SIZE         10      // Number of pending client connections
#define MAX_CLIENTS          10      // Number of clients the server can handle
#define POLL_TIMEOUT         10      // Poll() timeout duration

int main(int argc, char ** argv)
{
    int             exit_code = E_FAILURE;
    options_t       options   = { 0 };
    server_config_t config    = { 0 };
    options.p_value           = DEFAULT_PORT_NUMBER;
    options.n_value           = DEFAULT_THREAD_COUNT;

    if ((MIN_ARGS > argc) || (MAX_ARGS < argc))
    {
        PRINT_DEBUG("main(): Invalid number of arguments\n");
        goto END;
    }

    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARN("This is a warning");
    LOG_CRITICAL("Critical issue encountered: %s", strerror(errno));
    LOG_EMERG("Emergency: System failure!");

    // Initialize signal handler
    exit_code = signal_action_setup();
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("main(): Error initializing signal handler\n");
        goto END;
    }

    exit_code = process_options(argc, argv, &options);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("main(): Error processing options\n");
        goto END;
    }

    config.backlog_size = BACKLOG_SIZE;
    config.client_request =
        NULL; // TODO(nick): Build out client request function
    config.max_clients = MAX_CLIENTS;
    config.num_threads = options.n_value;
    config.port        = options.p_value;
    config.timeout     = POLL_TIMEOUT;

    exit_code = start_tcp_server(&config);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("main(): Error running TCP server\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
