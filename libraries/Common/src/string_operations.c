#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_operations.h"
#include "utilities.h"

int strip_newline(char * input_str_p)
{
    int exit_code = E_FAILURE;

    if (NULL == input_str_p)
    {
        print_error("strip_newline(): NULL argument passed.");
        goto END;
    }
    size_t len         = 0;
    size_t newline_pos = 0;

    len         = strlen(input_str_p);
    newline_pos = strcspn(input_str_p, "\n");

    if (newline_pos < len)
    {
        input_str_p[newline_pos] = '\0';
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int strings_are_equal(const char * str1_p, const char * str2_p, int * result_p)
{
    int    exit_code = E_FAILURE;
    int    result    = 0; // To hold the comparison result
    size_t len1      = 0;
    size_t len2      = 0;
    size_t max_len   = 0;
    int    check     = 0;

    if ((NULL == str1_p) || (NULL == str2_p) || (NULL == result_p))
    {
        print_error("strings_are_equal(): NULL argument passed.");
        goto END;
    }

    len1 = strlen(str1_p);
    len2 = strlen(str2_p);

    // Determine the longer length
    max_len = (len1 > len2) ? len1 : len2;

    // Compare strings up to the length of the longer string
    check = strncmp(str1_p, str2_p, max_len);
    if (0 == check)
    {
        result = 1; // Strings are equal
    }

    *result_p = result;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int copy_string(const char * input,
                char **      output,
                size_t       character_limit,
                bool         allow_truncation)
{
    int    exit_code      = E_FAILURE;
    size_t num_characters = 0;
    size_t buffer_size    = 0;
    char * buffer         = NULL;

    if ((NULL == input) || (NULL == output))
    {
        print_error("copy_string(): NULL argument passed.");
        goto END;
    }

    buffer_size = character_limit + 1; // +1 for NULL terminator

    buffer = calloc(buffer_size, sizeof(char));
    if (NULL == buffer)
    {
        print_error("copy_string(): buffer - CMR failure.");
        goto END;
    }

    num_characters = snprintf(buffer, buffer_size, "%s", input);
    if (num_characters >= buffer_size)
    {
        if (false == allow_truncation)
        {
            print_error("copy_string(): String truncation detected.");
            free(buffer);
            buffer = NULL;
            goto END;
        }
    }

    *output = buffer;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
