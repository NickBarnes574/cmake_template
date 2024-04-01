/**
 * @name Pointers - Pointer and Array
 * @file 2_pointer_and_array.c
 * @brief This example shows the relationship between arrays and pointers,
 * demonstrating how to access array elements using pointers.
 * @version 0.1
 * @date 2024-03-30
 *
 */

#include <stdio.h>

#include "utilities.h"

#define ARRAY_SIZE 5

int main()
{
    int array[] = {10, 20, 30, 40, 50};
    int *p_array = array; // Pointer to the first element of the array

    for (int idx = 0; idx < ARRAY_SIZE; idx++)
    {
        printf("array[%d] = %d ", idx, array[idx]);
        printf("*(p_array + %d) = %d\n", idx, *(p_array + idx));
    }

    return E_SUCCESS;
}

/*** end of file ***/
