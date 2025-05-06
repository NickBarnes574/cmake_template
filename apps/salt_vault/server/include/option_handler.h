/**
 * @file option_handler.h
 *
 * @brief
 */
#ifndef _OPTION_HANDLER_H
#define _OPTION_HANDLER_H

#include <stdbool.h>
#include <stdint.h> // int32_t

#define MAX_PORT_SIZE 6 // Maximum size (in characters) for the port string

/**
 * @struct options
 * @brief Structure to store command-line options.
 *
 * This structure is used to store the options passed via the command-line
 * arguments. It keeps track of whether certain flags are activated and stores
 * their corresponding values if they are provided.
 */
typedef struct options
{
    bool    n_flag;  // Flag - number of threads
    int32_t n_value; // Store the number of threads
    bool    p_flag;  // Flag - port number
    char *  p_value; // Store the port value as a string
} options_t;

/**
 * @brief Processes command-line options and populates an options_t structure.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array containing the command-line arguments.
 * @param options The address of a pointer to an options_t structure
 * where the options will be stored.
 * @return int - Returns E_SUCCESS on successful processing, otherwise
 * E_FAILURE.
 */
int process_options(int argc, char ** argv, options_t * options_p);

#endif /* _OPTION_HANDLER_H */

/*** end of file ***/
