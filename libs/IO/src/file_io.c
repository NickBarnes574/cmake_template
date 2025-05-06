// NOLINTNEXTLINE
#define _GNU_SOURCE // getline

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "file_io.h"
#include "utilities.h"

#define MIN_FD    3 // The lowest allowable user-defined file descriptor
#define MAX_BYTES (size_t)4096 // Max number of bytes to read or write at once

int read_line(FILE * read_fd, char ** line)
{
    int     exit_code           = E_FAILURE;
    ssize_t num_characters_read = 0;
    size_t  line_size;
    char *  line_buffer = NULL;

    if (NULL == line)
    {
        PRINT_DEBUG("read_line(): NULL line pointer address.\n");
        goto END;
    }

    errno               = 0;
    num_characters_read = getline(&line_buffer, &line_size, read_fd);
    if (-1 == num_characters_read)
    {
        if (0 != errno)
        {
            perror("read_line(): %d\n");
            free(line_buffer);  // Free the buffer if an error occurred.
            line_buffer = NULL; // Nullify the pointer after freeing.
            goto END;
        }
        // EOF reached if no error was detected.
        free(line_buffer);
        line_buffer = NULL;
        exit_code   = E_EOF;
        goto END;
    }

    *line = line_buffer;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int file_exists(const char * filepath)
{
    int exit_code = E_FAILURE;

    if (NULL == filepath)
    {
        PRINT_DEBUG("file_exists(): NULL filepath passed.\n");
        goto END;
    }

    exit_code = access(filepath, F_OK);
    if (E_SUCCESS != exit_code)
    {
        PRINT_DEBUG("file_exists(): Unable to access filepath.\n");
    }

END:
    return exit_code;
}

int safe_fclose(FILE * file)
{
    int exit_code = E_FAILURE;
    int result    = 0;

    if (NULL == file)
    {
        PRINT_DEBUG("safe_fclose(): NULL argument passed.\n");
        goto END;
    }

    result = fclose(file);
    if (EOF == result)
    {
        PRINT_DEBUG("safe_fclose(): Failed to close file.\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int safe_close(int file_fd)
{
    int exit_code = E_FAILURE;
    int result    = 0;

    result = close(file_fd);
    if (-1 == result)
    {
        PRINT_DEBUG("safe_close(): Error closing file descriptor.\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

char * safe_fgets(char * buffer, size_t size, FILE * stream)
{
    char * result = NULL;

    if ((NULL == buffer) || (NULL == stream))
    {
        // Log an appropriate error message
        PRINT_DEBUG("safe_fgets(): Invalid arguments passed.\n");
        goto END;
    }

    result = fgets(buffer, (int)size, stream);
    if (NULL == result)
    {
        if (feof(stream))
        {
            PRINT_DEBUG("safe_fgets(): End of file reached.\n");
        }
        else if (ferror(stream))
        {
            perror("safe_fgets(): Read error\n");
        }
    }

END:
    return result;
}

/*** end of file ***/
