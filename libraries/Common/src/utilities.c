#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utilities.h"

#define BYTE             8
#define TIME_BUFFER_SIZE 80

void print_error(const char * p_message)
{
    if (NULL == p_message)
    {
        goto END;
    }
#ifdef DEBUG
    /* [ACTION]: Suppressing fprintf() error.
     * [JUSTIFICATION]: Result inconsequential to any further operations.
     */
    // NOLINTNEXTLINE
    fprintf(stderr, "[ERROR]: %s\n", p_message);
#endif
END:
    return;
}

void print_strerror(const char * p_message)
{
    if (NULL == p_message)
    {
        goto END;
    }
#ifdef DEBUG
    /* [ACTION]: Suppressing fprintf() error.
     * [JUSTIFICATION]: Result inconsequential to any further operations.
     */
    // NOLINTNEXTLINE
    fprintf(stderr, "%s %s\n", p_message, strerror(errno));
#endif
END:
    return;
}

int message_log(const char * prefix_p,
                color_code_t color,
                log_dest_t   destination,
                const char * format,
                ...)
{
    int          exit_code            = E_FAILURE;
    char         buffer[MAX_MSG_SIZE] = { 0 };
    const char * color_code           = NULL;
    va_list      args;
    int          ret = 0;
    time_t       now = 0;
    struct tm *  timeinfo;
    char         time_buffer[TIME_BUFFER_SIZE] = { 0 };
    FILE *       file                          = NULL;

    // Timestamp
    now = time(NULL);
    if (now == ((time_t)-1))
    {
        fprintf(stderr, "message_log(): time() failed.\n");
        goto END;
    }

    timeinfo = localtime(&now);
    if (strftime(
            time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo) ==
        0)
    {
        fprintf(stderr,
                "message_log(): strftime() failed to format the time.\n");
        goto END;
    }

    // Handle color code
    switch (color)
    {
        case COLOR_YELLOW:
            color_code = "\033[0;33m";
            break;
        case COLOR_RED:
            color_code = "\033[0;31m";
            break;
        case COLOR_GREEN:
            color_code = "\033[0;32m";
            break;
        case COLOR_BLUE:
            color_code = "\033[0;34m";
            break;
        default:
            color_code = "\033[0m"; // Default to no color
            break;
    }

    // Prepare the message
    va_start(args, format);
    ret = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (ret >= (int)sizeof(buffer))
    {
        fprintf(stderr,
                "message_log(): Message truncated. Buffer size exceeded.\n");
        goto END;
    }

    // Handle log destination
    if (destination == LOG_FILE || destination == LOG_BOTH)
    {
        file = fopen(LOGFILE, "a");
        if (!file)
        {
            fprintf(stderr, "message_log(): Unable to open log file.\n");
            goto END;
        }
    }

    // Print the message with timestamp
    if (destination == LOG_CONSOLE || destination == LOG_BOTH)
    {
        printf("%s[%s][%s]: %s\033[0m\n",
               color_code,
               time_buffer,
               prefix_p,
               buffer);
    }
    if (destination == LOG_FILE || destination == LOG_BOTH)
    {
        fprintf(file, "[%s][%s]: %s\n", time_buffer, prefix_p, buffer);
        fclose(file);
    }

    exit_code = E_SUCCESS;

END:
    return exit_code;
}

/*** end of file ***/
