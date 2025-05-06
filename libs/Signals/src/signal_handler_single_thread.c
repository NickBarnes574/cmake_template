#include "signal_handler.h"
#include "utilities.h"

#include <string.h>
#include <unistd.h>

_Atomic int signal_flag = ACTIVE;

static void signal_handler(int sig)
{
    switch (sig)
    {
        case SIGINT:
            signal_flag = SHUTDOWN;
            write(STDOUT_FILENO, "\n" SIGINT_MSG, 1 + SIGINT_MSG_LEN);
            break;

        case SIGTERM:
            signal_flag = SHUTDOWN;
            write(STDOUT_FILENO, "\n" SIGTERM_MSG, 1 + SIGTERM_MSG_LEN);
            break;

        case SIGHUP:
            write(STDOUT_FILENO, "\n" SIGHUP_MSG, 1 + SIGHUP_MSG_LEN);
            break;

        default:
            break;
    }
}

int signal_action_setup(void)
{
    int              exit_code = E_FAILURE;
    struct sigaction sig_act;
    int              handled_signals[] = SIGNAL_LIST;

    memset(&sig_act, 0, sizeof(sig_act));
    sig_act.sa_handler = signal_handler;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;

    for (int idx = 0; idx < HANDLED_SIGNALS_COUNT; ++idx)
    {
        exit_code = sigaction(handled_signals[idx], &sig_act, NULL);
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("signal_action_setup(): sigaction failed");
            goto END;
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
