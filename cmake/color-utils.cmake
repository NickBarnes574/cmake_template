# ----------------------------------------------------------------------------
# BRIEF -- color-utils.cmake
# ----------------------------------------------------------------------------
# Provides colorized output functions and ANSI support for console messages.
#
# Features:
# - Defines ANSI escape codes for color output
# - Implements `message_color()` to print messages with consistent formatting
#
# Usage:
# - Call `message_color(STATUS/WARNING/ERROR "msg")` instead of `message()`
# ----------------------------------------------------------------------------

# SOURCE:
# https://stackoverflow.com/questions/18968979/how-to-make-colorized-message-with-cmake

if(NOT DEFINED USE_ANSI_COLORS)
    if(WIN32)
        set(USE_ANSI_COLORS OFF)
    else()
        set(USE_ANSI_COLORS ON)
    endif()
endif()

if(USE_ANSI_COLORS)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[m")
    set(ColourBold "${Esc}[1m")
    set(Red "${Esc}[31m")
    set(Green "${Esc}[32m")
    set(Yellow "${Esc}[33m")
    set(Blue "${Esc}[34m")
    set(Magenta "${Esc}[35m")
    set(Cyan "${Esc}[36m")
    set(White "${Esc}[37m")
    set(BoldRed "${Esc}[1;31m")
    set(BoldGreen "${Esc}[1;32m")
    set(BoldYellow "${Esc}[1;33m")
    set(BoldBlue "${Esc}[1;34m")
    set(BoldMagenta "${Esc}[1;35m")
    set(BoldCyan "${Esc}[1;36m")
    set(BoldWhite "${Esc}[1;37m")
endif()

# ----------------------------------------------------------------------------
# FUNCTION - message_color()
#
# Description:
# Prints colored and categorized CMake messages for better readability.
#
# Usage:
# message_color(<TYPE> <MESSAGE> ...)
# - TYPE: STATUS (default), WARNING, FATAL_ERROR, etc.
# - MESSAGE: Content to print with optional color variable
#
# Notes:
# - ANSI colors are enabled by default on non-Windows systems
# - STATUS messages may include a custom color as the first argument
# ----------------------------------------------------------------------------
function(message_color)
    set(options FATAL_ERROR SEND_ERROR WARNING STATUS AUTHOR_WARNING)
    set(MessageType STATUS) # Default message type
    set(Color "${Green}")   # Default STATUS message color

    list(FIND options "${ARGV0}" index)
    if(index GREATER -1)
        set(MessageType "${ARGV0}")
        list(REMOVE_AT ARGV 0)
    endif()

    # Check if a color argument was passed for STATUS messages
    if(MessageType STREQUAL STATUS AND ARGC GREATER 1)
        list(GET ARGV 0 CustomColor)
        if(DEFINED ${CustomColor})
            set(Color "${${CustomColor}}")
            list(REMOVE_AT ARGV 0)
        endif()
    endif()

    # Print messages with appropriate colors
    if(USE_ANSI_COLORS)
        if(MessageType STREQUAL FATAL_ERROR OR MessageType STREQUAL SEND_ERROR)
            message(${MessageType} "${BoldRed}${ARGV}${ColourReset}")
        elseif(MessageType STREQUAL WARNING)
            message(${MessageType} "${BoldYellow}${ARGV}${ColourReset}")
        elseif(MessageType STREQUAL AUTHOR_WARNING)
            message(${MessageType} "${BoldCyan}${ARGV}${ColourReset}")
        elseif(MessageType STREQUAL STATUS)
            message(${MessageType} "${Color}${ARGV}${ColourReset}")
        else()
            message("${ARGV}")
        endif()
    else()
        message(${MessageType} "${ARGV}")
    endif()
endfunction()

# *** END OF FILE ***