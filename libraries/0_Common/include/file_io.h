/**
 * @file file_io.h
 *
 * @brief A module for file I/O functions
 */
#ifndef _FILE_IO_H
#define _FILE_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/**
 * @brief Reads a line from a file.
 *
 * This function reads an entire line from a stream, storing the address of
 * the buffer containing the text into '*line'. The buffer is null-terminated
 * and includes the newline character, if one was found. If 'read_fd' or 'line'
 * is NULL, an error is reported.
 *
 * @param read_fd The file stream to read from.
 * @param line Pointer to the buffer where the line will be stored.
 * @return int Returns E_SUCCESS if a line is read successfully, or E_FAILURE on
 * failure.
 */
int read_line(FILE * read_fd, char ** line);

/**
 * @brief Checks if a file exists at the specified path.
 *
 * This function checks the existence of a file at the given 'filepath'. It
 * reports an error if 'filepath' is NULL or if the file cannot be accessed.
 *
 * @param filepath The path to the file whose existence is to be checked.
 * @return int Returns E_SUCCESS if the file exists, or E_FAILURE if it does not
 * or if an error occurs.
 */
int file_exists(const char * filepath);

#endif /* _FILE_IO_H */

/*** end of file ***/
