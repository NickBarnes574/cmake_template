# ----------------------------------------------------------------------------
# BRIEF -- adsan-config.cmake
# ----------------------------------------------------------------------------
# Configures AddressSanitizer support for detecting memory errors at runtime.
#
# Features:
# - Enables AddressSanitizer (`-fsanitize=address`) in Debug mode if `ENABLE_ADSAN` is ON
#
# Usage:
# - Add `-DENABLE_ADSAN=ON` to CMake configure command
# ----------------------------------------------------------------------------

# Enable AddressSanitizer if requested
if(ENABLE_ADSAN)
    message(STATUS "Address Sanitizer [ENABLED]")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING "" FORCE)
endif()

# *** END OF FILE ***
