#include <stdbool.h>

#include "signal_handler.h"
#include "utilities.h"

typedef enum
{
    NO_SIGNAL        = 0,
    USER_SIGNAL      = 1,
    INTERRUPT_SIGNAL = 2
} signal_type_t;

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
            signal_flag_g = USER_SIGNAL;
            break;

        case SIGINT:
            signal_flag_g = INTERRUPT_SIGNAL;
            break;

        default:
            signal_flag_g = NO_SIGNAL;
            break;
    }
}

void reset_signal_flag()
{
    signal_flag_g = NO_SIGNAL;
}

int check_for_signals(void)
{
    int current_signal = -1;

    if (NO_SIGNAL != signal_flag_g)
    {
        current_signal = signal_flag_g;
        reset_signal_flag(); // Reset signal flag to handle next signals cleanly

        switch (current_signal)
        {
            case INTERRUPT_SIGNAL:
            case USER_SIGNAL:
                return SHUTDOWN; // Define SHUTDOWN appropriately
        }
    }
    return CONTINUE_RUNNING; // Define CONTINUE_RUNNING appropriately
}