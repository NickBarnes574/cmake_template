#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#define _XOPEN_SOURCE 700 // prevent incomplete class type "struct sigaction"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    CONTINUE_RUNNING = 0, // Indicates operations should continue running
    SHUTDOWN         = 1  // Indicates a shutdown signal has been received
} signal_response_t;

/**
 * @brief Global signal flag to store a value set by the signal handler.
 * Default value should be 0
 **/
extern volatile sig_atomic_t signal_flag_g;
extern pthread_mutex_t       signal_flag_mutex;

/**
 * @brief This will be the function used to handle the signals.
 * The following signals will modify the global variable "signal_flag_g"
 * SIGUSER1 : This signal should set signal_flag_g to 1
 * SIGINT : This signal should set signal_flag_g to 2
 * The default value of "signal_flag_g" should be 0
 *
 * You must use `sigaction()`, and not `signal()`
 *
 * @param signal The signal that was raised.
 * @return No return value.
 */
void signal_handler(int signal);

/** This function is used to set up the signal handling. You must use
 * `sigaction()`, and not `signal()`
 *
 * @return SUCCESS: SUCCESS
 *         FAILURE: ERROR
 */
int signal_action_setup(void);

int check_for_signals(void);

void reset_signal_flag();

#endif
