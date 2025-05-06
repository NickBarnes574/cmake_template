/**
 * @file option_handler.c
 * @brief Option Handling for the Server Application
 *
 * This file contains functions for processing command-line options and printing
 * the help menu. It is responsible for setting up initial configurations based
 * on the options passed to the application.
 */
// NOLINTNEXTLINE
#define _GNU_SOURCE // for strnlen() //

#include <getopt.h> // getopt()
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strnlen()
#include <unistd.h> // getopt()

#include "number_converter.h" // str_to_int32()
#include "option_handler.h"
#include "utilities.h"

#define MIN_NUM_THREADS 2     // Minimum number of threads allowed
#define MAX_PORT_VALUE  65535 // Maximum allowable port number
#define MIN_PORT_VALUE  1025  // Minimum allowable port number

//
// -----------------------------UTILITY FUNCTIONS-----------------------------
//

/**
 * @brief Prints the help menu to the console.
 *
 * The function provides information on how to use the server application,
 * detailing available options and their usage.
 */
static void print_help_menu();

//
// ------------------------------REPORT FUNCTIONS------------------------------
//

/**
 * @brief Report any additional arguments passed to the program.
 *
 * This function is responsible for checking if there are any extra arguments
 * that `getopt` didn't process. If there are, it reports them to the stderr.
 *
 * @param optind Index of the next element to be processed in argv.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 *
 * @return E_SUCCESS if no additional invalid arguments are found, E_FAILURE
 * otherwise.
 */
static int report_extra_arguments(int optind, int argc, char ** argv);

/**
 * @brief Reports invalid or incomplete options to stderr.
 *
 * This function is called when an unknown option or an option requiring an
 * argument (like `-n` or `-p`) is passed without one. It prints an appropriate
 * error message to stderr.
 */
static void report_invalid_options();

//
// ------------------------------OPTION FUNCTIONS------------------------------
//

/**
 * @brief Process the '-p' command-line option.
 *
 * This function is responsible for processing the '-p' option passed in the
 * command line. The '-p' option specifies the port number on which the server
 * should listen.
 *
 * @param optarg Pointer to the string containing the argument for the '-p'
 * option.
 * @param options_p Pointer to the options_t struct where the parsed value
 * should be stored.
 *
 * @return E_SUCCESS on successful processing, E_FAILURE otherwise.
 */
static int process_p_option(char * optarg, options_t * options_p);

/**
 * @brief Process the '-n' command-line option.
 *
 * This function is responsible for processing the '-n' option passed in the
 * command line. The '-n' option specifies the number of threads in the thread
 * pool.
 *
 * @param optarg Pointer to the string containing the argument for the '-n'
 * option.
 * @param options_p Pointer to the options_t struct where the parsed value
 * should be stored.
 *
 * @return E_SUCCESS on successful processing, E_FAILURE otherwise.
 */
static int process_n_option(char * optarg, options_t * options_p);

// +---------------------------------------------------------------------------+
// |                            MAIN OPTION HANDLER                            |
// +---------------------------------------------------------------------------+

int process_options(int argc, char ** argv, options_t * options_p)
{
    int exit_code = E_FAILURE;
    int option    = 0;

    if ((NULL == argv) || (NULL == *argv) || (NULL == options_p))
    {
        PRINT_DEBUG("process_options(): NULL argument passed.\n");
        goto END;
    }

    // Generally assignments inside conditional statements go against Barr-C and
    // CSD-T coding standards, however the following conditional was pulled
    // directly from the 'getopt()' man page, and follows a more idomatic
    // approach that is more recognizable to C programmers who are familiar with
    // 'getopt()'.
    while (-1 != (option = getopt(argc, argv, "n:p:h")))
    {
        switch (option)
        {
            case 'p':
                exit_code = process_p_option(optarg, options_p);
                if (E_SUCCESS != exit_code)
                {
                    PRINT_DEBUG("Unable to process 'p' option.\n");
                    goto END;
                }
                break;

            case 'n':
                exit_code = process_n_option(optarg, options_p);
                if (E_SUCCESS != exit_code)
                {
                    PRINT_DEBUG("Unable to process 'n' option.\n");
                    goto END;
                }
                break;

            case 'h':
                goto END;
                break;

            case '?':
                report_invalid_options();
                exit_code = E_FAILURE;
                goto END;
                break;

            default:
                // NOLINTNEXTLINE
                fprintf(stderr, "Unknown option encountered.\n");
                exit_code = E_FAILURE;
                goto END;
        }
    }

    exit_code = report_extra_arguments(optind, argc, argv);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("Invalid arguments passed.\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    if (E_FAILURE == exit_code)
    {
        print_help_menu();
    }
    return exit_code;
}

// *****************************************************************************
//                          STATIC FUNCTION DEFINITIONS
// *****************************************************************************

static int process_n_option(char * optarg, options_t * options_p)
{
    int     exit_code   = E_FAILURE;
    int32_t num_threads = 0;

    if ((NULL == optarg) || (NULL == options_p))
    {
        PRINT_DEBUG("process_n_option(): NULL argument passed.\n");
        goto END;
    }

    // Check if the 'n' flag was already set
    if (true == options_p->n_flag)
    {
        PRINT_DEBUG("process_n_option(): '-n' flag already true.\n");
        goto END;
    }
    else
    {
        options_p->n_flag = true;
    }

    // Convert string to integer
    exit_code = str_to_int32(optarg, &num_threads);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG(
            "process_n_option(): Unable to convert 'n_value' to number.\n");
        goto END;
    }

    if (MIN_NUM_THREADS > num_threads)
    {
        PRINT_DEBUG(
            "process_n_option(): Number of threads must be 2 or more.\n");

        exit_code = E_FAILURE;
        goto END;
    }

    options_p->n_value = num_threads;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int process_p_option(char * optarg, options_t * options_p)
{
    int     exit_code   = E_FAILURE;
    int32_t port_number = 0;

    if ((NULL == optarg) || (NULL == options_p))
    {
        PRINT_DEBUG("process_p_option(): NULL argument passed.\n");
        goto END;
    }

    // Check if the 'p' flag was already set
    if (true == options_p->p_flag)
    {
        PRINT_DEBUG("process_p_option(): '-p' flag already true.\n");
        goto END;
    }
    else
    {
        options_p->p_flag = true;
    }

    // Convert string to integer and check range
    exit_code = str_to_int32(optarg, &port_number);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG(
            "process_p_option(): Unable to convert 'p_value' to number.\n");
        goto END;
    }

    if ((MAX_PORT_VALUE < port_number) || (MIN_PORT_VALUE > port_number))
    {
        PRINT_DEBUG("process_p_option(): Port number out of range.\n");
        exit_code = E_FAILURE;
        goto END;
    }

    options_p->p_value = optarg;

    exit_code = E_SUCCESS;

END:
    return exit_code;
}

static void report_invalid_options()
{
    if (('n' == optopt) || ('p' == optopt))
    {
        // NOLINTNEXTLINE
        fprintf(stderr, "Option '-%c' requires an argument.\n", optopt);
    }
    else
    {
        // NOLINTNEXTLINE
        fprintf(stderr, "Unknown option '-%c'.\n", optopt);
    }
}

static int report_extra_arguments(int optind, int argc, char ** argv)
{
    int exit_code = E_FAILURE;

    if ((NULL == argv) || (NULL == *argv))
    {
        PRINT_DEBUG("NULL argument passed.\n");
        goto END;
    }

    if (optind < argc)
    {
        // NOLINTNEXTLINE
        fprintf(stderr, "Invalid arguments encountered: ");
        while (optind < argc)
        {
            // NOLINTNEXTLINE
            fprintf(stderr, "%s ", argv[optind++]);
        }
        // NOLINTNEXTLINE
        fprintf(stderr, "\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void print_help_menu()
{
    printf("Admin Server - Water Treatment Simulator\n");
    printf("----------------------------------------------------------\n");
    printf("Usage: ./admin_server [options]\n");
    printf("Options:\n");
    printf(
        "  -p PORT   Port to listen on; (MIN: 1025, MAX: 65535) defaults to "
        "31337.\n");
    printf(
        "  -n NUM    Number of threads in the pool; (MIN: 2) defaults to 4.\n");
    printf("  -h        Print this help menu and exit.\n");
    printf("\n");
    printf("Description:\n");
    printf(
        "  This is the main control server for the water treatment "
        "simulator.\n");
    printf("\n");
    printf("Examples:\n");
    printf("------------------------NO ARGUMENTS:\n");
    printf("  ./admin_server\n");
    printf("\n------------------------WITH ARGUMENTS:\n");
    printf("  ./admin_server -p 8080\n");
    printf("  ./admin_server -n 8\n");
    printf("  ./admin_server -p 8080 -n 8\n");
    printf("\n------------------------HELP MENU:\n");
    printf("  ./admin_server -h\n");
    printf("\n");
    printf("For more information, see the documentation.\n");
}

/*** end of file ***/
