/**
 * @name Pointers - Basic Pointer Usage
 * @file 1_basic_pointer_usage.c
 * @brief This example demonstrates how to declare, initialize, and use
 * pointers to change the value of a variable.
 * @version 0.1
 * @date 2024-03-30
 *
 */

#include <stdio.h>

#include "utilities.h"

int main()
{
    int   number   = 10;
    int * p_number = NULL;

    p_number = &number;

    printf("Value of number: %d\n", number);
    printf("Address of number: %p\n", (void *)&number);
    printf("Value of p_number: %p\n", (void *)p_number);
    printf("Value pointed to by p_number: %d\n", *p_number);

    *p_number = 20; // Changing the value of number through the pointer
    printf("New value of number: %d\n", number);

    return E_SUCCESS;
}

/*** end of file ***/
