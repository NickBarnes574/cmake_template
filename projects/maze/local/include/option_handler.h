/**
 * @file option_handler.h
 * @brief Header for Option Handling Functions
 *
 * This header file provides the interface for processing command-line options
 * and creating a structure to store these options for NetCalc.
 *
 */
#ifndef _OPTION_HANDLER_H
#define _OPTION_HANDLER_H

#include <stdbool.h> // bool
#include <stdint.h>  // for uint32_t

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
    bool     d_flag;  // Flag for doors
    bool     D_flag;  // Flag for wall-digging
    bool     w_flag;  // Flag for water
    bool     m_flag;  // Flag for maze generation
    uint32_t m_value; // Stores the size of the maze to be generated
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
int process_options(int         argc,
                    char **     argv,
                    int         max_args,
                    options_t * options_p);

#endif /* _OPTION_HANDLER_H */
/*** end of file ***/
