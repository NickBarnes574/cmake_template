/**
 * @file utilities.h
 *
 * @brief Module for utilities for wide-variety of use cases
 */

#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h> // strrchr()

// Exit codes
#define E_SUCCESS           0
#define E_FAILURE           -1
#define E_EOF               -2
#define E_NULL_POINTER      -3
#define E_CONNECTION_CLOSED -4

extern pthread_mutex_t stdout_mutex;

#ifdef DEBUG

#define RED   "\x1B[31m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define PRINT_DEBUG(...)             \
    do                               \
    {                                \
        printf("%s@ %s%s:%s%d%s | ", \
               RED,                  \
               __FILENAME__,         \
               RESET,                \
               GREEN,                \
               __LINE__,             \
               RESET);               \
        printf(__VA_ARGS__);         \
    } while (0);

#else
#define PRINT_DEBUG(...)     \
    do                       \
    {                        \
        printf(__VA_ARGS__); \
    } while (0);

#endif // DEBUG

#endif /* _UTILITIES_H */

/*** end of file ***/
