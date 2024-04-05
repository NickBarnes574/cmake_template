/**
 * @name Pointers - Passing Pointers to Functions
 * @file 3_passing_pointers_to_functions.c
 * @brief This example illustrates how to use pointers to modify the value of
 * variables passed to a function.
 * @version 0.1
 * @date 2024-03-30
 *
 */

#include <stdio.h>

#include "utilities.h"

int addTen(int *p_num)
{
    int exit_code = E_FAILURE;

    if (NULL == p_num)
    {
        print_error("add_ten(): NULL argument passed.");
        goto END;
    }

    *p_num += 10; // Add 10 to the value num points to

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int main()
{
    int exit_code = E_FAILURE;
    int value = 5;

    printf("Value before function call: %d\n", value);

    exit_code = addTen(&value); // Pass the address of value to the function
    if (E_SUCCESS != exit_code)
    {
        print_error("main(): Unable to add ten to value.");
        goto END;
    }

    printf("Value after function call: %d\n", value);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}