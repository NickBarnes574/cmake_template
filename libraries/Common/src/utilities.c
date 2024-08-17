#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "system_info.h"
#include "utilities.h"

#define BYTE             8
#define TIME_BUFFER_SIZE 80
#define BUFFER_SIZE      1024

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

void print_boxed_info(FILE *       output,
                      const char * header,
                      const char * hostname,
                      const char * os,
                      const char * cpu,
                      const char * architecture,
                      const char * memory)
{
    size_t max_len = strlen(header);

    // Calculate the max length considering the full strings
    char temp[BUFFER_SIZE];
    snprintf(temp, BUFFER_SIZE, "Hostname: %s", hostname);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    snprintf(temp, BUFFER_SIZE, "Operating System: %s", os);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    snprintf(temp, BUFFER_SIZE, "CPU: %s", cpu);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    snprintf(temp, BUFFER_SIZE, "Architecture: %s", architecture);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    snprintf(temp, BUFFER_SIZE, "Memory: %s", memory);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    max_len += 2; // Padding

    fprintf(output, "+");
    for (size_t i = 0; i < max_len + 2; i++)
    {
        fprintf(output, "-");
    }
    fprintf(output, "+\n");

    fprintf(output, "| %-*s |\n", (int)max_len, header);

    fprintf(output, "+");
    for (size_t i = 0; i < max_len + 2; i++)
    {
        fprintf(output, "-");
    }
    fprintf(output, "+\n");

    snprintf(temp, BUFFER_SIZE, "Hostname: %s", hostname);
    fprintf(output, "| %-*s |\n", (int)max_len, temp);

    snprintf(temp, BUFFER_SIZE, "Operating System: %s", os);
    fprintf(output, "| %-*s |\n", (int)max_len, temp);

    snprintf(temp, BUFFER_SIZE, "CPU: %s", cpu);
    fprintf(output, "| %-*s |\n", (int)max_len, temp);

    snprintf(temp, BUFFER_SIZE, "Architecture: %s", architecture);
    fprintf(output, "| %-*s |\n", (int)max_len, temp);

    snprintf(temp, BUFFER_SIZE, "Memory: %s", memory);
    fprintf(output, "| %-*s |\n", (int)max_len, temp);

    fprintf(output, "+");
    for (size_t i = 0; i < max_len + 2; i++)
    {
        fprintf(output, "-");
    }
    fprintf(output, "+\n");
}

int log_system_info()
{
    int    exit_code = E_FAILURE;
    FILE * file      = NULL;
    char * hostname  = NULL;
    char * os        = NULL;
    char * cpu       = NULL;
    char * arch      = NULL;
    char * memory    = NULL;

    hostname = get_hostname();
    os       = get_operating_system();
    cpu      = get_cpu_info();
    arch     = get_cpu_architecture();
    memory   = get_memory_info();

    file = fopen(LOGFILE, "a");
    if (!file)
    {
        fprintf(stderr, "log_system_info(): Unable to open log file.\n");
        goto END;
    }

    print_boxed_info(file,
                     "SYSTEM INFO",
                     hostname ? hostname : "N/A",
                     os ? os : "N/A",
                     cpu ? cpu : "N/A",
                     arch ? arch : "N/A",
                     memory ? memory : "N/A");
    fclose(file);

    print_boxed_info(stdout,
                     "SYSTEM INFO",
                     hostname ? hostname : "N/A",
                     os ? os : "N/A",
                     cpu ? cpu : "N/A",
                     arch ? arch : "N/A",
                     memory ? memory : "N/A");

    exit_code = E_SUCCESS;

END:
    free(hostname);
    free(os);
    free(cpu);
    free(arch);
    free(memory);
    return exit_code;
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

    if (0 == strncmp(prefix_p, "SESSION_START", 25))
    {
        fprintf(file,
                "\n---------------------------------SESSION "
                "START---------------------------------\n\n");
        fclose(file);
        exit_code = E_SUCCESS;
        goto END;
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

void noop_free(void * data)
{
    // No operation
    (void)data;
}

/*** end of file ***/
