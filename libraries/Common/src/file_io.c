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
        print_error("read_line(): NULL line pointer address.");
        goto END;
    }

    errno               = 0;
    num_characters_read = getline(&line_buffer, &line_size, read_fd);
    if (-1 == num_characters_read)
    {
        if (0 != errno)
        {
            print_strerror("read_line():");
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
        print_error("NULL filepath passed.");
        goto END;
    }

    exit_code = access(filepath, F_OK);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to access filepath.");
    }

END:
    return exit_code;
}

/*** end of file ***/
