#include <stdarg.h> // va_list

#include "file_io.h"
#include "runtime_log.h"
#include "system_info.h"
#include "threadsafe_io.h"
#include "utilities.h"

#define TIME_BUFFER_SIZE 80
#define COLOR_CODE_SIZE  32
#define BUFFER_SIZE      1024

pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;

static const int fg_ansi_codes[] = {
    39,                         // FG_DEFAULT
    31, 32, 33, 34, 35, 36, 37, // FG_RED to FG_WHITE
    91, 92, 93, 94, 95, 96, 97  // FG_BRIGHT_RED to FG_BRIGHT_WHITE
};

static const int bg_ansi_codes[] = {
    49,                               // BG_NONE
    41,  42,  43,  44,  45,  46,  47, // BG_RED to BG_WHITE
    101, 102, 103, 104, 105, 106, 107 // BG_BRIGHT_RED to BG_BRIGHT_WHITE
};

static bool get_timestamp(char * buffer, size_t size);
static int  get_color_code(log_color_t color, char * buffer, size_t size);
static void print_boxed_info(FILE *       output,
                             const char * header,
                             const char * hostname,
                             const char * operating_system,
                             const char * cpu,
                             const char * architecture,
                             const char * memory);

int message_log(const char * prefix,
                fg_color_t   fg_color,
                bg_color_t   bg_color,
                log_dest_t   destination,
                bool         show_time,
                const char * format,
                ...)
{
    int         exit_code                     = E_FAILURE;
    int         check_val                     = -1;
    char        msg_buffer[MAX_MSG_SIZE]      = { 0 };
    char        time_buffer[TIME_BUFFER_SIZE] = { 0 };
    char        color_code[COLOR_CODE_SIZE]   = { 0 };
    log_color_t log_color                     = { 0 };
    FILE *      file                          = NULL;
    va_list     args;

    if ((NULL == prefix) || (NULL == format))
    {
        PRINT_DEBUG("message_log(): NULL argument passed.\n");
        goto END;
    }

    pthread_mutex_lock(&stdout_mutex);

    log_color.foreground = fg_color;
    log_color.background = bg_color;

    exit_code = get_color_code(log_color, color_code, sizeof(color_code));
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("message_log(): get_color_code() failed.\n");
        pthread_mutex_unlock(&stdout_mutex);
        goto END;
    }

    if (true == show_time)
    {
        exit_code = get_timestamp(time_buffer, sizeof(time_buffer));
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("message_log(): get_timestamp() failed.\n");
            pthread_mutex_unlock(&stdout_mutex);
            goto END;
        }
    }

    // Format user message
    va_start(args, format);
    check_val = vsnprintf(msg_buffer, sizeof(msg_buffer), format, args);
    if (0 > check_val)
    {
        exit_code = E_FAILURE;
        PRINT_DEBUG("message_log(): vsnprintf() failed.\n");
        va_end(args);
        pthread_mutex_unlock(&stdout_mutex);
        goto END;
    }
    va_end(args);

    if ((LOG_FILE == destination) || (LOG_BOTH == destination))
    {
        file = fopen(LOGFILE, "ae");
        if (NULL == file)
        {
            PRINT_DEBUG("message_log(): Unable to open log file.\n");
            pthread_mutex_unlock(&stdout_mutex);
            goto END;
        }
    }

    // Print to console
    if ((destination == LOG_CONSOLE) || (destination == LOG_BOTH))
    {
        if (true == show_time)
        {
            printf("%s[%s][%s]: %s\033[0m\n",
                   color_code,
                   time_buffer,
                   prefix,
                   msg_buffer);
        }
        else
        {
            printf("%s[%s]: %s\033[0m\n", color_code, prefix, msg_buffer);
        }
    }

    // Print to file
    if (((destination == LOG_FILE) || (destination == LOG_BOTH)) &&
        (NULL != file))
    {
        if (true == show_time)
        {
            check_val = fprintf(
                file, "[%s][%s]: %s\n", time_buffer, prefix, msg_buffer);
            if (0 > check_val)
            {
                PRINT_DEBUG("message_log(): fprintf() failure.\n");
                goto END;
            }
        }
        else
        {
            check_val = fprintf(file, "[%s]: %s\n", prefix, msg_buffer);
            if (0 > check_val)
            {
                PRINT_DEBUG("message_log(): fprintf() failure.\n");
                goto END;
            }
        }
        exit_code = safe_fclose(file);
        if (E_SUCCESS != exit_code)
        {
            PRINT_DEBUG("message_log(): safe_fclose() failure.\n");
            goto END;
        }
    }

    pthread_mutex_unlock(&stdout_mutex);

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int log_system_info()
{
    int    exit_code        = E_FAILURE;
    FILE * file             = NULL;
    char * hostname         = NULL;
    char * operating_system = NULL;
    char * cpu              = NULL;
    char * arch             = NULL;
    char * memory           = NULL;

    hostname         = get_hostname();
    operating_system = get_operating_system();
    cpu              = get_cpu_info();
    arch             = get_cpu_architecture();
    memory           = get_memory_info();

    file = fopen(LOGFILE, "ae");
    if (!file)
    {
        (void)fprintf(stderr, "log_system_info(): Unable to open log file.\n");
        goto END;
    }

    print_boxed_info(file,
                     "SYSTEM INFO",
                     hostname ? hostname : "N/A",
                     operating_system ? operating_system : "N/A",
                     cpu ? cpu : "N/A",
                     arch ? arch : "N/A",
                     memory ? memory : "N/A");
    (void)fclose(file);

    print_boxed_info(stdout,
                     "SYSTEM INFO",
                     hostname ? hostname : "N/A",
                     operating_system ? operating_system : "N/A",
                     cpu ? cpu : "N/A",
                     arch ? arch : "N/A",
                     memory ? memory : "N/A");

    exit_code = E_SUCCESS;

END:
    free(hostname);
    free(operating_system);
    free(cpu);
    free(arch);
    free(memory);
    return exit_code;
}

static bool get_timestamp(char * buffer, size_t size)
{
    bool        result        = false;
    time_t      current_time  = { 0 };
    struct tm * time_info     = { 0 };
    size_t      chars_written = 0;

    if (NULL == buffer)
    {
        PRINT_DEBUG("get_timestamp(): NULL argument passed.\n");
        goto END;
    }

    current_time = time(NULL);
    if (E_FAILURE == current_time)
    {
        PRINT_DEBUG("get_timestamp(): time() error.\n");
        goto END;
    }

    time_info = localtime(&current_time);
    if (NULL == time_info)
    {
        PRINT_DEBUG("get_timestamp(): localtime() error.\n");
        goto END;
    }

    chars_written = strftime(buffer, size, "%Y-%m-%d %H:%M:%S", time_info);
    if (0 == chars_written)
    {
        PRINT_DEBUG("get_timestamp(): strftime() error.\n");
        goto END;
    }

    result = true;
END:
    return result;
}

static int get_color_code(log_color_t color, char * buffer, size_t size)
{
    int exit_code     = E_FAILURE;
    int chars_written = 0;

    if (NULL == buffer)
    {
        PRINT_DEBUG("get_color_code(): NULL argument passed.\n");
        goto END;
    }

    chars_written = snprintf(buffer,
                             size,
                             "\033[%d;%dm",
                             fg_ansi_codes[color.foreground],
                             bg_ansi_codes[color.background]);

    if (-1 == chars_written)
    {
        PRINT_DEBUG("get_color_code(): snprintf() error.\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

void print_boxed_info(FILE *       output,
                      const char * header,
                      const char * hostname,
                      const char * operating_system,
                      const char * cpu,
                      const char * architecture,
                      const char * memory)
{
    size_t max_len = strlen(header);

    // Calculate the max length considering the full strings
    char temp[BUFFER_SIZE];
    (void)snprintf(temp, BUFFER_SIZE, "Hostname: %s", hostname);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    (void)snprintf(temp, BUFFER_SIZE, "Operating System: %s", operating_system);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    (void)snprintf(temp, BUFFER_SIZE, "CPU: %s", cpu);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    (void)snprintf(temp, BUFFER_SIZE, "Architecture: %s", architecture);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    (void)snprintf(temp, BUFFER_SIZE, "Memory: %s", memory);
    max_len = max_len > strlen(temp) ? max_len : strlen(temp);

    max_len += 2; // Padding

    (void)fprintf(output, "+");
    for (size_t i = 0; i < max_len + 2; i++)
    {
        (void)fprintf(output, "-");
    }
    (void)fprintf(output, "+\n");

    (void)fprintf(output, "| %-*s |\n", (int)max_len, header);

    (void)fprintf(output, "+");
    for (size_t i = 0; i < max_len + 2; i++)
    {
        (void)fprintf(output, "-");
    }
    (void)fprintf(output, "+\n");

    (void)snprintf(temp, BUFFER_SIZE, "Hostname: %s", hostname);
    (void)fprintf(output, "| %-*s |\n", (int)max_len, temp);

    (void)snprintf(temp, BUFFER_SIZE, "Operating System: %s", operating_system);
    (void)fprintf(output, "| %-*s |\n", (int)max_len, temp);

    (void)snprintf(temp, BUFFER_SIZE, "CPU: %s", cpu);
    (void)fprintf(output, "| %-*s |\n", (int)max_len, temp);

    (void)snprintf(temp, BUFFER_SIZE, "Architecture: %s", architecture);
    (void)fprintf(output, "| %-*s |\n", (int)max_len, temp);

    (void)snprintf(temp, BUFFER_SIZE, "Memory: %s", memory);
    (void)fprintf(output, "| %-*s |\n", (int)max_len, temp);

    (void)fprintf(output, "+");
    for (size_t i = 0; i < max_len + 2; i++)
    {
        (void)fprintf(output, "-");
    }
    (void)fprintf(output, "+\n");
}

/*** end of file ***/
