#include <stdio.h>
#include <stdlib.h>

#include "reverse_array.h"
#include "utilities.h"

int reverse_array(int * arr, size_t length)
{
    int exit_code = E_FAILURE;  // Initialize exit_code to E_FAILURE
    int start     = 0;          // Start index for the reversal process
    int end       = length - 1; // End index for the reversal process
    int temp      = 0;          // Temporary variable for swapping

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
        temp       = arr[start]; // Store the start element in temp
        arr[start] = arr[end];   // Move the end element to the start position
        arr[end] = temp; // Move the temp value (original start element) to the
                         // end position
        start++;         // Move the start index forward
        end--;           // Move the end index backward
    }

    exit_code =
        E_SUCCESS; // Set exit_code to E_SUCCESS if reversal is successful
END:
    return exit_code; // Return the exit code
}