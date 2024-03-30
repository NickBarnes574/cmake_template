#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "maze.h"
#include "option_handler.h"
#include "utilities.h"

#define MIN_ARGS        2 // Min number of CLI arguments needed for the program to run
#define MAX_ARGS        7 // Max number of CLI arguments the program can handle
#define NON_OPTION_ARGS 1 // Number of allowable non-option arguments

int main(int argc, char ** argv)
{
    int       exit_code  = E_FAILURE;
    options_t options    = { 0 };
    char *    filepath_p = NULL;

    if ((MIN_ARGS > argc) || (MAX_ARGS < argc))
    {
        print_error("main(): Invalid number of arguments.");
        goto END;
    }

    exit_code = process_options(argc, argv, NON_OPTION_ARGS, &options);
    if (E_SUCCESS != exit_code)
    {
        print_error("main(): Unable to process options.");
        goto END;
    }

    filepath_p = argv[1];

    exit_code = process_maze(filepath_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("main(): Unable to process maze.");
        goto END;
    }

END:
    return exit_code;
}

/*** end of file ***/
