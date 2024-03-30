#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "number_generator.h"
#include "utilities.h"

int random_number(int lower_bound, int upper_bound, int * result_p)
{
    int        exit_code   = E_FAILURE;
    static int is_seed_set = 0;

    if (NULL == result_p)
    {
        print_error("random_number(): NULL argument passed.");
        goto END;
    }

    if ((INT32_MAX < lower_bound) || (INT32_MIN > lower_bound))
    {
        print_error("random_number(): lower bound out of range.");
        goto END;
    }

    if ((INT32_MAX < upper_bound) || (INT32_MIN > upper_bound))
    {
        print_error("random_number(): upper bound out of range.");
        goto END;
    }

    if (lower_bound > upper_bound)
    {
        print_error(
            "random_number(): lower bound is greater than upper "
            "bound.");
        goto END;
    }

    if (0 == is_seed_set)
    {
        srand(time(NULL));
        is_seed_set = 1;
    }

    *result_p = lower_bound + rand() % ((upper_bound - lower_bound) + 1);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
