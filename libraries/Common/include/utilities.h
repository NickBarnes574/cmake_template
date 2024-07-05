/**
 * @file utilities.h
 *
 * @brief Module for utilities for wide-variety of use cases
 */

#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <stdbool.h>
#include <stdint.h>

// Exit codes
#define E_SUCCESS           0
#define E_FAILURE           -1
#define E_EOF               -2
#define E_NULL_POINTER      -3
#define E_CONNECTION_CLOSED -4

// Color codes
typedef enum
{
    COLOR_NONE,
    COLOR_YELLOW,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE
} color_code_t;

typedef enum
{
    LOG_CONSOLE,
    LOG_FILE,
    LOG_BOTH
} log_dest_t;

#define MAX_MSG_SIZE 500
#define LOGFILE      "server.log"

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
#define PRINT_DEBUG(...)    \
    do                      \
    {                       \
        printf(__VA_ARGS__) \
    } while (0);

#endif // DEBUG

/**
 * @brief Prints a custom error message to `stderr` using `fprintf()`.
 *
 * @param p_message The message to print
 * @return void - Returns nothing
 */
void print_error(const char * p_message);

/**
 * @brief Uses the underlying `strerror()` function to get a human-readable
 * error message corresponding to the `errno` value, and prints a custom error
 * message to `stderr` using `fprintf()`.
 *
 * @param p_message The message to print
 * @return void - Returns nothing
 */
void print_strerror(const char * p_message);

/**
 * @brief Logs a formatted message with a timestamp, prefix, and colored output.
 *
 * This function takes a variable number of arguments to support formatting,
 * similar to printf. It constructs a timestamped message string with the
 * specified color and prefix. The message is then printed to the standard
 * output. If the formatted message exceeds the maximum message size, an error
 * is reported. The color of the output can be specified using the 'color'
 * parameter, which is of type 'color_code_t'.
 *
 * @param prefix_p A constant character pointer to the prefix of the message.
 * @param color An enum value of 'color_code_t' that specifies the color of the
 * output.
 * @param format A format string as would be given to printf.
 * @param ... Variadic arguments to be formatted within the message.
 * @return int Returns E_SUCCESS on successful logging, or E_FAILURE if an error
 * occurs.
 */
int message_log(const char * prefix_p,
                color_code_t color,
                log_dest_t   destination,
                const char * format,
                ...);

int log_system_info();

void noop_free(void * data);

#endif /* _UTILITIES_H */

/*** end of file ***/
