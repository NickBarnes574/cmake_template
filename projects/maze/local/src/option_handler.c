/**
 * @file option_handler.c
 * @brief Option Handling for the Server Application
 *
 * This file contains functions for processing command-line options and printing
 * the help menu. It is responsible for setting up initial configurations based
 * on the options passed to the application.
 */
// #define _GNU_SOURCE // for strnlen()

#include <getopt.h> // getopt()
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strnlen()
#include <unistd.h> // getopt()

#include "number_converter.h" // str_to_uint32()
#include "option_handler.h"
#include "utilities.h"

#define MIN_MAZE_SIZE 4 // Minimum allowable size for a generated maze

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
static int report_extra_arguments(int     max_args,
                                  int     optind,
                                  int     argc,
                                  char ** argv);

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
 * @brief
 * NAME: Doors
 * OPTION: -d
 * DESCRIPTION:
 * Adds support for "doors", triggered with the -d command-line option. Closed
 * doors are '+' characters in the map file, open doors are '/'. If the printed
 * path passes through a door, the door must be open. Otherwise, the door’s
 * state should be unchanged from the input file. It takes one extra "step" to
 * open a door that is closed, which may affect the shortest path.
 *
 * @param optarg Pointer to the string containing the argument for the '-d'
 * option.
 * @param options_p Pointer to the options_t struct where the parsed value
 * should be stored.
 *
 * @return E_SUCCESS on successful processing, E_FAILURE otherwise.
 */
static int process_d_option(char * optarg, options_t * options_p);

/**
 * @brief
 * NAME: Wall-digging
 * OPTION: -D
 * DESCRIPTION:
 * Adds support for wall-digging, triggered with the -D command-line option. Any
 * wall may be dug through, which takes ten times as long as a normal step
 * through a regular part of the floor (space character). So the path may go
 * through a wall, but there may be a more efficient, longer path without
 * digging. Notice that the "dig" action (that takes ten steps) doesn’t move the
 * @ into the space dug out; it takes an extra step to actually move into the
 * dug-out space.
 *
 * @param optarg Pointer to the string containing the argument for the '-D'
 * option.
 * @param options_p Pointer to the options_t struct where the parsed value
 * should be stored.
 *
 * @return E_SUCCESS on successful processing, E_FAILURE otherwise.
 */
static int process_D_option(char * optarg, options_t * options_p);

/**
 * @brief
 * NAME: Water
 * OPTION: -w
 * DESCRIPTION:
 * Adds support for water, triggered with the -w command-line option. Water,
 * which shows up as '~', takes thrice as long to traverse as a regular part of
 * the floor (space character). So the path may go over it, but there might be a
 * more efficient, longer path on land.
 *
 * @param optarg Pointer to the string containing the argument for the '-w'
 * option.
 * @param options_p Pointer to the options_t struct where the parsed value
 * should be stored.
 *
 * @return E_SUCCESS on successful processing, E_FAILURE otherwise.
 */
static int process_w_option(char * optarg, options_t * options_p);

/**
 * @brief
 * NAME: Maze Generation
 * OPTION: -m
 * DESCRIPTION:
 * Adds support for a map generator, triggered with the -m sz command-line
 * option. It should build a solvable square map of dimensions sz × sz and print
 * it with the solution.
 *
 * @param optarg Pointer to the string containing the argument for the '-m'
 * option.
 * @param options_p Pointer to the options_t struct where the parsed value
 * should be stored.
 *
 * @return E_SUCCESS on successful processing, E_FAILURE otherwise.
 */
static int process_m_option(char * optarg, options_t * options_p);

// +---------------------------------------------------------------------------+
// |                            MAIN OPTION HANDLER                            |
// +---------------------------------------------------------------------------+

int process_options(int argc, char ** argv, int max_args, options_t * options_p)
{
    int exit_code = E_FAILURE;
    int option    = 0;

    if ((NULL == argv) || (NULL == *argv) || (NULL == options_p))
    {
        print_error("process_options(): NULL argument passed.");
        goto END;
    }

    // Generally assignments inside conditional statements go against Barr-C and
    // CSD-T coding standards, however the following conditional was pulled
    // directly from the 'getopt()' man page, and follows a more idomatic
    // approach that is more recognizable to C programmers who are familiar with
    // 'getopt()'.
    while (-1 != (option = getopt(argc, argv, "dDwm:h")))
    {
        switch (option)
        {
            case 'd':
                exit_code = process_d_option(optarg, options_p);
                if (E_SUCCESS != exit_code)
                {
                    print_error("Unable to process 'p' option.");
                    goto END;
                }
                break;

            case 'D':
                exit_code = process_D_option(optarg, options_p);
                if (E_SUCCESS != exit_code)
                {
                    print_error("Unable to process 'n' option.");
                    goto END;
                }
                break;

            case 'w':
                exit_code = process_w_option(optarg, options_p);
                if (E_SUCCESS != exit_code)
                {
                    print_error("Unable to process 'n' option.");
                    goto END;
                }
                break;

            case 'm':
                exit_code = process_m_option(optarg, options_p);
                if (E_SUCCESS != exit_code)
                {
                    print_error("Unable to process 'n' option.");
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
                fprintf(stderr, "Unknown option encountered.\n");
                exit_code = E_FAILURE;
                goto END;
        }
    }

    exit_code = report_extra_arguments(max_args, optind, argc, argv);
    if (E_SUCCESS != exit_code)
    {
        print_error("Invalid arguments passed.");
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

static int process_d_option(char * optarg, options_t * options_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == optarg) || (NULL == options_p))
    {
        print_error("process_d_option(): NULL argument passed.");
        goto END;
    }

    // Check if the 'd' flag was already set
    if (true == options_p->d_flag)
    {
        print_error("process_d_option(): '-d' flag already true.");
        goto END;
    }
    else
    {
        options_p->d_flag = true;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int process_D_option(char * optarg, options_t * options_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == optarg) || (NULL == options_p))
    {
        print_error("process_D_option(): NULL argument passed.");
        goto END;
    }

    // Check if the 'D' flag was already set
    if (true == options_p->D_flag)
    {
        print_error("process_D_option(): '-D' flag already true.");
        goto END;
    }
    else
    {
        options_p->D_flag = true;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int process_w_option(char * optarg, options_t * options_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == optarg) || (NULL == options_p))
    {
        print_error("process_w_option(): NULL argument passed.");
        goto END;
    }

    // Check if the 'w' flag was already set
    if (true == options_p->w_flag)
    {
        print_error("process_w_option(): '-w' flag already true.");
        goto END;
    }
    else
    {
        options_p->w_flag = true;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int process_m_option(char * optarg, options_t * options_p)
{
    int      exit_code = E_FAILURE;
    uint32_t maze_size = 0;

    if ((NULL == optarg) || (NULL == options_p))
    {
        print_error("process_m_option(): NULL argument passed.");
        goto END;
    }

    // Check if the 'm' flag was already set
    if (true == options_p->m_flag)
    {
        print_error("process_m_option(): '-m' flag already true.");
        goto END;
    }
    else
    {
        options_p->m_flag = true;
    }

    // Convert string to integer
    exit_code = str_to_uint32(optarg, &maze_size);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "process_m_option(): Unable to convert 'm_value' to number.");
        goto END;
    }

    if (MIN_MAZE_SIZE > maze_size)
    {
        print_error("process_m_option(): Maze must be 4 x 4 or larger.");

        exit_code = E_FAILURE;
        goto END;
    }

    options_p->m_value = maze_size;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void report_invalid_options()
{
    if ('m' == optopt)
    {
        fprintf(stderr, "Option '-%c' requires an argument.\n", optopt);
    }
    else
    {
        fprintf(stderr, "Unknown option '-%c'.\n", optopt);
    }
}

static int report_extra_arguments(int     max_args,
                                  int     optind,
                                  int     argc,
                                  char ** argv)
{
    int exit_code = E_FAILURE;

    if ((NULL == argv) || (NULL == *argv))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if ((optind + max_args) < argc)
    {
        fprintf(stderr, "Invalid arguments encountered: ");
        while (optind < argc)
        {
            fprintf(stderr, "%s ", argv[optind++]);
        }
        fprintf(stderr, "\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void print_help_menu()
{
    printf("Maze Explorer\n");
    printf("----------------------------------------\n");
    printf("Usage: ./maze <filename> [options]\n");
    printf("Options:\n");
    printf(
        "  -d        Enable door handling; closed doors are '+', open doors "
        "are '/'.\n"
        "            It takes an extra step to open a door that is closed.\n");
    printf(
        "  -D        Enable wall-digging; walls can be dug through, taking ten "
        "times\n"
        "            as long as a regular step. Requires additional step to "
        "enter dug-out space.\n");
    printf(
        "  -w        Enable water traversal; traversing water '~' takes thrice "
        "as long\n"
        "            as moving over a regular space.\n");
    printf(
        "  -m <size> Generate a maze of specified size; solvable square map of "
        "dimensions\n"
        "            size × size. Minimum size is 4.\n");
    printf("  -h        Print this help menu and exit.\n");
    printf("\n");
    printf("Description:\n");
    printf(
        "  The Maze Explorer is an application that demonstrates various "
        "traversal\n"
        "  techniques in a maze, including door handling, wall-digging, and "
        "water traversal.\n");
    printf("\n");
    printf("Examples:\n");
    printf("------------------------NO ARGUMENTS (DEFAULT BEHAVIOR):\n");
    printf("  ./maze <filename>\n");
    printf("\n------------------------WITH ARGUMENTS:\n");
    printf("  ./maze <filename> -d\n");
    printf("  ./maze <filename> -D\n");
    printf("  ./maze <filename> -w\n");
    printf("  ./maze <filename> -m 10\n");
    printf("  ./maze <filename> -d -D -m 10\n");
    printf("\n------------------------HELP MENU:\n");
    printf("  ./maze <filename> -h\n");
    printf(
        "\nFor more information, see the documentation or contact support.\n");
}

/*** end of file ***/
