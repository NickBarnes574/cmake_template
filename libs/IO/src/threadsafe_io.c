#include "threadsafe_io.h"
#include "utilities.h" // for PRINT_DEBUG()

void ts_printf(const char * format, ...)
{
    va_list args;
    pthread_mutex_lock(&stdout_mutex);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    pthread_mutex_unlock(&stdout_mutex);
}

int ts_snprintf(char * buffer, size_t buffer_size, const char * format, ...)
{
    int     exit_code = E_FAILURE;
    size_t  result    = 0;
    va_list args;

    if ((NULL == buffer) || (NULL == format))
    {
        PRINT_DEBUG("ts_snprintf(): NULL argument passed.");
        goto END;
    }

    pthread_mutex_lock(&stdout_mutex);
    va_start(args, format);
    result = vsnprintf(buffer, buffer_size, format, args);
    va_end(args);
    pthread_mutex_unlock(&stdout_mutex);

    if (result >= buffer_size)
    {
        PRINT_DEBUG("ts_snprintf(): Output truncated.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int ts_fprintf(FILE * file, const char * format, ...)
{
    int     exit_code = E_FAILURE;
    int     result    = 0;
    va_list args;

    if ((NULL == file) || (NULL == format))
    {
        PRINT_DEBUG("ts_fprintf(): NULL argument passed.");
        goto END;
    }

    // Lock only if stdout or stderr
    if ((file == stdout) || (file == stderr))
    {
        pthread_mutex_lock(&stdout_mutex);
    }

    va_start(args, format);
    result = vfprintf(file, format, args);
    va_end(args);

    if ((file == stdout) || (file == stderr))
    {
        pthread_mutex_unlock(&stdout_mutex);
    }

    if (0 > result)
    {
        PRINT_DEBUG("ts_fprintf(): Failed to write to file.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
