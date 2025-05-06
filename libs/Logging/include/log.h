#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h> // strrchr()

#ifdef NDEBUG
#define LOG_LEVEL 0 // Disable all logging when NDEBUG is set
#else
#define LOG_LEVEL 6 // Adjust this for different max logging levels
#endif

// Define log levels
#define LOG_LEVEL_DEBUG    6
#define LOG_LEVEL_TRACE    5
#define LOG_LEVEL_INFO     4
#define LOG_LEVEL_WARN     3
#define LOG_LEVEL_CRITICAL 2
#define LOG_LEVEL_EMERG    1

// Define ANSI color codes
#define LOG_COLOR_RESET  "\x1B[0m"
#define LOG_COLOR_RED    "\x1B[31m"
#define LOG_COLOR_YELLOW "\x1B[33m"
#define LOG_COLOR_BLUE   "\x1B[34m"
#define LOG_COLOR_CYAN   "\x1B[36m"
#define LOG_COLOR_WHITE  "\x1B[97m"
#define LOG_COLOR_BOLD   "\x1B[1m"

// Extract filename from full path
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// Core logging macro
#define LOG(level, color, level_str, ...)   \
    do                                      \
    {                                       \
        if (level <= LOG_LEVEL)             \
        {                                   \
            printf("%s[%s]%s %s:%d (%s): ", \
                   color,                   \
                   level_str,               \
                   LOG_COLOR_RESET,         \
                   __FILENAME__,            \
                   __LINE__,                \
                   __func__);               \
            printf(__VA_ARGS__);            \
            printf("\n");                   \
        }                                   \
    } while (0)

// Logging level macros
#define LOG_DEBUG(...) LOG(LOG_LEVEL_DEBUG, LOG_COLOR_CYAN, "DBG", __VA_ARGS__)
#define LOG_TRACE(...) LOG(LOG_LEVEL_TRACE, LOG_COLOR_BLUE, "TRC", __VA_ARGS__)
#define LOG_INFO(...)  LOG(LOG_LEVEL_INFO, LOG_COLOR_WHITE, "INF", __VA_ARGS__)
#define LOG_WARN(...)  LOG(LOG_LEVEL_WARN, LOG_COLOR_YELLOW, "WRN", __VA_ARGS__)
#define LOG_CRITICAL(...) \
    LOG(LOG_LEVEL_CRITICAL, LOG_COLOR_RED, "CRT", __VA_ARGS__)
#define LOG_EMERG(...) \
    LOG(LOG_LEVEL_EMERG, LOG_COLOR_BOLD LOG_COLOR_RED, "EMG", __VA_ARGS__)

#endif // LOG_H

/*** end of file ***/