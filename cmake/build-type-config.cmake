# ----------------------------------------------------------------------------
# BRIEF -- build-type-config.cmake
# ----------------------------------------------------------------------------
# Ensures a default CMake build type is selected.
#
# Features:
# - Defaults to `Release` if no build type is specified
# - Uses `message_color()` to print current build mode
#
# Usage:
# - Included automatically in top-level `CMakeLists.txt`
# ----------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/color-utils.cmake)

# Default to Release mode if not set
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type (Debug, Release, Test)" FORCE)
endif()

# Display build type
message_color(STATUS "*** Building in ${CMAKE_BUILD_TYPE} mode ***")

# *** END OF FILE ***