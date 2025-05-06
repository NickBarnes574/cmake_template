/**
 * @file message_log.h
 *
 * @brief A full-featured logging system supporting colored output, timestamps,
 * and file logging.
 */
#ifndef _MESSAGE_LOG_H
#define _MESSAGE_LOG_H

#include <stdbool.h>

// Color codes
typedef enum fg_color
{
    FG_DEFAULT,
    FG_RED,
    FG_GREEN,
    FG_YELLOW,
    FG_BLUE,
    FG_MAGENTA,
    FG_CYAN,
    FG_WHITE,
    FG_BRIGHT_RED,
    FG_BRIGHT_GREEN,
    FG_BRIGHT_YELLOW,
    FG_BRIGHT_BLUE,
    FG_BRIGHT_MAGENTA,
    FG_BRIGHT_CYAN,
    FG_BRIGHT_WHITE
} fg_color_t;

typedef enum bg_color
{
    BG_NONE,
    BG_RED,
    BG_GREEN,
    BG_YELLOW,
    BG_BLUE,
    BG_MAGENTA,
    BG_CYAN,
    BG_WHITE,
    BG_BRIGHT_RED,
    BG_BRIGHT_GREEN,
    BG_BRIGHT_YELLOW,
    BG_BRIGHT_BLUE,
    BG_BRIGHT_MAGENTA,
    BG_BRIGHT_CYAN,
    BG_BRIGHT_WHITE
} bg_color_t;

typedef struct
{
    fg_color_t foreground;
    bg_color_t background;
} log_color_t;

typedef enum
{
    LOG_CONSOLE,
    LOG_FILE,
    LOG_BOTH
} log_dest_t;

#define MAX_MSG_SIZE 500
#define LOGFILE      "server.log"

/**
 * @brief Logs a formatted message with a timestamp, prefix, and colored output.
 *
 * This function takes a variable number of arguments to support formatting,
 * similar to printf. It constructs a timestamped message string with the
 * specified color and prefix. The message is then printed to the standard
 * output. If the formatted message exceeds the maximum message size, an error
 * is reported. The color of the output can be specified using the 'color'
 * parameter, which is of type 'fg_color_t'.
 *
 * @param prefix_p A constant character pointer to the prefix of the message.
 * @param color An enum value of 'fg_color_t' that specifies the color of the
 * output.
 * @param format A format string as would be given to printf.
 * @param ... Variadic arguments to be formatted within the message.
 * @return int Returns E_SUCCESS on successful logging, or E_FAILURE if an error
 * occurs.
 */
int message_log(const char * prefix,
                fg_color_t   fg_color,
                bg_color_t   bg_color,
                log_dest_t   destination,
                bool         show_time,
                const char * format,
                ...);

int log_system_info();

#endif /* _MESSAGE_LOG_H */

/*** end of file ***/
