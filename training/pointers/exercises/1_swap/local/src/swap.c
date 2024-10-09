#include <stdio.h>

#include "utilities.h"

int swap(int * p_num_1, int * p_num_2)
{
    int exit_code = E_FAILURE;
    int temp      = 0;

    if ((NULL == p_num_1) || (NULL == p_num_2))
    {
        exit_code = E_NULL_POINTER;
        goto END;
    }

    temp     = *p_num_1;
    *p_num_1 = *p_num_2;
    *p_num_2 = temp;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}