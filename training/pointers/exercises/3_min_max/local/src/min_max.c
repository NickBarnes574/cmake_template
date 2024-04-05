#include <stdio.h>

#include "min_max.h"
#include "utilities.h"

int find_min_max(const int *p_arr, int length, int *p_min, int *p_max)
{
    int exit_code = E_FAILURE; // Initialize exit_code to E_FAILURE

    // Validate input parameters
    if ((NULL == p_arr) || (length <= 0) || (NULL == p_min) || (NULL == p_max))
    {
        print_error("find_min_max(): Invalid argument passed.");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    *p_min = p_arr[0]; // Initialize minimum to the first element of the array
    *p_max = p_arr[0]; // Initialize maximum to the first element of the array

    for (int idx = 1; idx < length; idx++)
    {
        if (p_arr[idx] < *p_min)
        {
            *p_min = p_arr[idx]; // Update minimum
        }
        else if (p_arr[idx] > *p_max)
        {
            *p_max = p_arr[idx]; // Update maximum
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code; // Return the exit code
}
