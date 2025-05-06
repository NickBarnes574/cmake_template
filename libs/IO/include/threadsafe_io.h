/**
 * @file threadsafe_io.h
 *
 * @brief
 */
#ifndef _THREADSAFE_IO_H
#define _THREADSAFE_IO_H

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

extern pthread_mutex_t stdout_mutex;

/**
 * Thread-safe printf to stdout
 */
void ts_printf(const char * format, ...);

/**
 * Safe snprintf with truncation check
 */
int ts_snprintf(char * buffer, size_t buffer_size, const char * format, ...);

/**
 * Thread-safe fprintf — locks stdout/stderr only
 */
int ts_fprintf(FILE * file, const char * format, ...);

#endif /* _THREADSAFE_IO_H */

/*** end of file ***/
