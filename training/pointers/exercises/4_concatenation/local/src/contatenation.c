#include "concatenation.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int str_concat(const char *p_str1, const char *p_str2, char **pp_result)
{
    int exit_code = E_FAILURE; // Initialize exit_code to E_FAILURE
    int length_str1, length_str2, total_length;

    // Validate input parameters
    if ((NULL == p_str1) || (NULL == p_str2) || (NULL == pp_result))
    {
        print_error("str_concat(): Invalid argument passed.");
        goto END;
    }

    // Calculate the length of both strings
    length_str1 = strnlen(p_str1, MAX_STR_LEN);
    length_str2 = strnlen(p_str2, MAX_STR_LEN);

    // Calculate total length of the new string (+1 for the null terminator)
    total_length = length_str1 + length_str2 + 1;

    // Allocate memory for the new string
    *pp_result = calloc(total_length, sizeof(char));
    if (NULL == *pp_result)
    {
        print_error("str_concat(): Memory allocation failed.");
        goto END;
    }

    // Copy the first string into the new string
    strncpy(*pp_result, p_str1, length_str1 + 1);

    // Concatenate the second string to the new string
    strncat(*pp_result, p_str2, length_str2 + 1);

    exit_code = E_SUCCESS;
END:
    return exit_code; // Return the exit code
}

/*** end of file ***/
