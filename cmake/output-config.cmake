# ----------------------------------------------------------------------------
# BRIEF -- output-config.cmake
# ----------------------------------------------------------------------------
# Detects system architecture and sets default build type.
#
# Features:
# - Sets `ARCH` based on host architecture (override with `-DARCH=<value>`)
# - Ensures `CMAKE_BUILD_TYPE` is defined for single-config generators
#
# Usage:
# - Include before defining paths or using ARCH-specific logic
# ----------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/paths-config.cmake)

# Detect architecture if not already set
if(NOT DEFINED ARCH)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|armv8.*)$")
        set(ARCH aarch64)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(armv7.*|armv6.*)$")
        set(ARCH arm)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
        set(ARCH x86_64)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv64)$")
        set(ARCH riscv64)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ppc64|powerpc64)$")
        set(ARCH ppc64)
    else()
        message(WARNING "Unknown architecture: ${CMAKE_SYSTEM_PROCESSOR}, defaulting to generic.")
        set(ARCH generic)
    endif()
endif()

# Set default build type for single-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type (Debug, Release, Test)" FORCE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${REMOTE_STATIC_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${REMOTE_DYNAMIC_LIB_DIR})

# *** END OF FILE ***
