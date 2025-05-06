#include "signal_handler.h"
#include "utilities.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

pthread_cond_t  signal_flag_cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t signal_flag_mutex = PTHREAD_MUTEX_INITIALIZER;

void * signal_wait_thread(void * arg)
{
    sigset_t blocked;
    int      sig       = 0;
    int      exit_code = E_FAILURE;

    (void)arg;

    sigemptyset(&blocked);
    int signals[] = SIGNAL_LIST;
    for (int i = 0; i < HANDLED_SIGNALS_COUNT; ++i)
    {
        sigaddset(&blocked, signals[i]);
    }

    // Block until signal is received
    exit_code = sigwait(&blocked, &sig);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("signal_wait_thread(): sigwait() failed");
        pthread_exit(NULL);
    }

    atomic_store(&signal_flag, SHUTDOWN);
    pthread_mutex_lock(&signal_flag_mutex);
    pthread_cond_broadcast(&signal_flag_cond); // Wake anyone waiting
    pthread_mutex_unlock(&signal_flag_mutex);

    switch (sig)
    {
        case SIGINT:
            write(STDOUT_FILENO, "\n" SIGINT_MSG, 1 + SIGINT_MSG_LEN);
            break;
        case SIGTERM:
            write(STDOUT_FILENO, "\n" SIGTERM_MSG, 1 + SIGTERM_MSG_LEN);
            break;
        case SIGHUP:
            write(STDOUT_FILENO, "\n" SIGHUP_MSG, 1 + SIGHUP_MSG_LEN);
            break;
        default:
            break;
    }

    pthread_exit(NULL);
}

/*** end of file ***/
