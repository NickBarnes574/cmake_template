#include <stdio.h>

#include "utilities.h"

int swap(int *num_1, int *num_2)
{
    int exit_code = E_FAILURE;
    int temp = 0;

    if ((NULL == num_1) || (NULL == num_2))
    {
        print_error("swap(): NULL argument passed.");
        goto END;
    }

    temp = *num_1;
    *num_1 = *num_2;
    *num_2 = temp;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}