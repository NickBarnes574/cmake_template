/**
 * @name Pointers - Dynamic Memory Allocation
 * @file 4_dynamic_memory_allocation.c
 * @brief This example demonstrates how to dynamically allocate memory for an
 * array using calloc and then free it using free().
 * @version 0.1
 * @date 2024-03-30
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"

#define ARRAY_SIZE 5

int main()
{
    int exit_code = E_FAILURE;
    int *p_array = NULL;

    // Dynamically allocate memory for 5 integers
    p_array = calloc(ARRAY_SIZE, sizeof(int));
    if (NULL == p_array)
    {
        print_error("main(): p_array - CMR failure.");
        goto END;
    }

    // Initialize array
    for (int idx = 0; idx < ARRAY_SIZE; idx++)
    {
        p_array[idx] = idx * idx;
    }

    // Print array
    for (int idx = 0; idx < ARRAY_SIZE; idx++)
    {
        printf("%d ", p_array[idx]);
    }
    printf("\n");

    // Free the allocated memory
    free(p_array);
    p_array = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}