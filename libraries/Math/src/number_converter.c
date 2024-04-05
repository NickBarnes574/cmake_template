#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // snprintf()

#include "number_converter.h"
#include "utilities.h"

#define BASE       10
#define UINT32_LEN 11

int str_to_int32(const char * input_p, int32_t * output_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == input_p) || (NULL == output_p))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    char * endptr = NULL;

    errno       = 0;
    long result = strtol(input_p, &endptr, BASE);

    // Check if there was a reported error when the result is equal to 0
    if ((0 != errno) && (0 == result))
    {
        print_error("strtol() unknown error.");
        goto END;
    }

    // Check if there was a reported error when the result is equal to LONG_MAX
    if ((ERANGE == errno) && (LONG_MAX == result))
    {
        print_error("signed 32-bit overflow detected.");
        goto END;
    }

    // Check if the result is greater than INT32_MAX
    if (INT32_MAX < result)
    {
        print_error("signed 32-bit integer overflow detected.");
        goto END;
    }

    // Check if the result is less than INT32_MIN
    if (INT32_MIN > result)
    {
        print_error("signed 32-bit integer underflow detected.");
        goto END;
    }

    // Check if no digits were found
    if (endptr == input_p)
    {
        print_error("strtol() invalid input - no digits found.");
        goto END;
    }

    // Check if there are any extra characters by dereferencing the pointer to
    // the first character that can't be converted
    if ('\0' != *endptr)
    {
        print_error("strtol() invalid input - extra characters.");
        goto END;
    }

    *output_p = (int32_t)result;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int str_to_uint32(const char * input_p, uint32_t * output_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == input_p) || (NULL == output_p))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    char * endptr = NULL;

    errno                = 0;
    unsigned long result = strtoul(input_p, &endptr, BASE);

    // Check if there was a reported error when the result is equal to 0
    if ((0 != errno) && (0 == result))
    {
        print_error("strtoul() unknown error.");
        goto END;
    }

    // Check if there was a reported error when the result is equal to ULONG_MAX
    if ((ERANGE == errno) && (ULONG_MAX == result))
    {
        print_error("unsigned long overflow detected.");
        goto END;
    }

    // Check if the result is greater than UINT32_MAX
    if (UINT32_MAX < result)
    {
        print_error("unsigned 32-bit integer overflow detected.");
        goto END;
    }

    // Check if no digits were found
    if (endptr == input_p)
    {
        print_error("strtoul() invalid input - no digits found.");
        goto END;
    }

    // Check if there are any extra characters by dereferencing the pointer to
    // the first character that can't be converted
    if ('\0' != *endptr)
    {
        print_error("strtoul() invalid input - extra characters.");
        goto END;
    }

    *output_p = (uint32_t)result;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int str_to_uint16(const char * input_p, uint16_t * output_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == input_p) || (NULL == output_p))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    char * endptr = NULL;

    errno                = 0;
    unsigned long result = strtoul(input_p, &endptr, BASE);

    // Check if there was a reported error when the result is equal to 0
    if ((0 != errno) && (0 == result))
    {
        print_error("strtoul() unknown error.");
        goto END;
    }

    // Check if there was a reported error when the result is equal to ULONG_MAX
    if ((ERANGE == errno) && (ULONG_MAX == result))
    {
        print_error("unsigned long overflow detected.");
        goto END;
    }

    // Check if the result is greater than UINT16_MAX
    if (UINT16_MAX < result)
    {
        print_error("unsigned 16-bit integer overflow detected.");
        goto END;
    }

    // Check if no digits were found
    if (endptr == input_p)
    {
        print_error("strtoul() invalid input - no digits found.");
        goto END;
    }

    // Check if there are any extra characters by dereferencing the pointer to
    // the first character that can't be converted
    if ('\0' != *endptr)
    {
        print_error("strtoul() invalid input - extra characters.");
        goto END;
    }

    *output_p = (uint16_t)result;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int uint32_to_str(uint32_t input, char ** output_p)
{
    int exit_code = E_FAILURE;
    int result    = 0;

    if (NULL == output_p)
    {
        print_error("uint32_to_str(): NULL argument passed.");
        goto END;
    }

    *output_p = calloc(UINT32_LEN, sizeof(char));
    if (NULL == *output_p)
    {
        print_error("uint32_to_str(): CMR failure.");
        goto END;
    }

    // Convert the number to a string
    result = snprintf(*output_p, UINT32_LEN, "%u", input);
    if (0 > result)
    {
        print_error("uint32_to_str(): snprintf() failure.");
        free(*output_p);
        *output_p = NULL;
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
