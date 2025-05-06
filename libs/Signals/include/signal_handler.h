#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#define _XOPEN_SOURCE 700 // prevent incomplete class type "struct sigaction"

#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>

#define SHUTDOWN 1
#define ACTIVE   0

#define HANDLED_SIGNALS_COUNT 3
#define SIGNAL_LIST           { SIGINT, SIGTERM, SIGHUP }

// -----------------------------------------
// Signal Messages (for async-signal-safe use)
// -----------------------------------------
#define SIGINT_MSG "[Signal] SIGINT received. Initiating graceful shutdown...\n"
#define SIGTERM_MSG \
    "[Signal] SIGTERM received. Initiating graceful shutdown...\n"
#define SIGHUP_MSG "[Signal] SIGHUP received. Reload signal ignored (no-op).\n"

#define SIGINT_MSG_LEN  (sizeof(SIGINT_MSG) - 1)
#define SIGTERM_MSG_LEN (sizeof(SIGTERM_MSG) - 1)
#define SIGHUP_MSG_LEN  (sizeof(SIGHUP_MSG) - 1)

// -----------------------------------------
// Global signal flag
// -----------------------------------------
extern _Atomic int signal_flag;

// -----------------------------------------
// Global condition and mutex (Used to pause specific threads if required)
// -----------------------------------------
extern pthread_cond_t  signal_flag_cond;
extern pthread_mutex_t signal_flag_mutex;

/**
 * @brief Set up a traditional signal handler (single-threaded).
 *
 * @return E_SUCCESS on success, E_FAILURE otherwise.
 */
int signal_action_setup(void);

/**
 * @brief Entry point for signal handling thread (multi-threaded).
 *
 * Blocks in sigwait() and sets signal_flag safely.
 *
 * @param arg Unused (NULL).
 * @return void* Always NULL.
 */
void * signal_wait_thread(void * arg);

#endif /* SIGNAL_HANDLER_H */

/*** end of file ***/