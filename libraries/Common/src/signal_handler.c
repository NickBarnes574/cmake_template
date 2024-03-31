#include <stdbool.h>

#include "signal_handler.h"
#include "utilities.h"

volatile sig_atomic_t signal_flag_g = false;

int signal_action_setup(void)
{
    int exit_code = E_FAILURE;

    struct sigaction action = { 0 };

    action.sa_handler = signal_handler;

    exit_code = sigaction(SIGUSR1, &action, NULL);
    if (E_SUCCESS != exit_code)
    {
        print_error("signal_action_setup(): 'SIGUSR1' sigaction failed.");
        goto END;
    }

    exit_code = sigaction(SIGINT, &action, NULL);
    if (E_SUCCESS != exit_code)
    {
        print_error("signal_action_setup(): 'SIGINT' sigaction failed.");
        goto END;
    }

END:
    return exit_code;
}

void signal_handler(int signal)
{
    switch (signal)
    {
        case SIGUSR1:
            signal_flag_g = 1;
            break;

        case SIGINT:
            signal_flag_g = 2;
            break;

        default:
            signal_flag_g = 0;
            break;
    }
}
