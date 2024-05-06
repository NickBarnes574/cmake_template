#include <stdio.h>
#include <stdlib.h>

#include "signal_handler.h"
#include "tcp_server.h"
#include "utilities.h"

int main(void)
{
    int exit_code = E_FAILURE;

    // Initialize signal handler
    exit_code = signal_action_setup();
    if (E_SUCCESS != exit_code)
    {
        print_error("main(): Unable to setup signal handler.");
        goto END;
    }

    exit_code = start_tcp_server("17337");
    if (E_SUCCESS != exit_code)
    {
        print_error("main(): start_tcp_server() failed.");
        goto END;
    }

END:
    return exit_code;
}
