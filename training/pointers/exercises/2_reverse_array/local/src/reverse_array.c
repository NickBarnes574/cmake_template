#include <stdio.h>
#include <stdlib.h>

#include "reverse_array.h"
#include "utilities.h"

int reverse_array(int * arr, size_t length)
{
    int exit_code = E_FAILURE;
    int start     = 0;
    int end       = length - 1;
    int temp      = 0;

    // Check for NULL pointer
    if (NULL == arr)
    {
        print_error("reverse_array(): NULL argument passed.");
        exit_code = E_NULL_POINTER;
        goto END;
    }

    if (length == 0)
    {
        print_error("reverse_array(): length must be greater than 0.");
        goto END;
    }

    // Reverse the array
    while (start < end)
    {
        temp       = arr[start];
        arr[start] = arr[end];
        arr[end]   = temp;
        start++;
        end--;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}