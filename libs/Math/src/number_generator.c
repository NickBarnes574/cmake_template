// NOLINTNEXTLINE
#define _GNU_SOURCE // random() srandom()

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "number_generator.h"
#include "utilities.h"

int random_number(int lower_bound, int upper_bound, int * result_p)
{
    int          exit_code   = E_FAILURE;
    static int   is_seed_set = 0;
    unsigned int seed        = 0;
    int          file_fd     = 0;
    int          rand_val    = 0;

    if (NULL == result_p)
    {
        PRINT_DEBUG("random_number(): NULL argument passed.");
        goto END;
    }

    if ((INT32_MAX < lower_bound) || (INT32_MIN > lower_bound))
    {
        PRINT_DEBUG("random_number(): lower bound out of range.");
        goto END;
    }

    if ((INT32_MAX < upper_bound) || (INT32_MIN > upper_bound))
    {
        PRINT_DEBUG("random_number(): upper bound out of range.");
        goto END;
    }

    if (lower_bound > upper_bound)
    {
        PRINT_DEBUG(
            "random_number(): lower bound is greater than upper bound.");
        goto END;
    }

    if (0 == is_seed_set)
    {
        // Use a more secure seed source (e.g., /dev/urandom)
        file_fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
        if (-1 != file_fd)
        {
            if (read(file_fd, &seed, sizeof(seed)) == sizeof(seed))
            {
                srandom(seed);
            }
            else
            {
                // Fallback if read fails
                srandom(time(NULL) ^ getpid());
            }
            close(file_fd);
        }
        else
        {
            // Fallback if /dev/urandom is unavailable
            srandom(time(NULL) ^ getpid());
        }
        is_seed_set = 1;
    }

    rand_val = (int)random();
    if (rand_val > INT_MAX)
    {
        PRINT_DEBUG("random_number(): Random value out of int range.");
        goto END;
    }

    *result_p = lower_bound + (rand_val % ((upper_bound - lower_bound) + 1));

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
