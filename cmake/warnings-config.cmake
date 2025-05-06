# ----------------------------------------------------------------------------
# BRIEF -- warnings-config.cmake
# ----------------------------------------------------------------------------
# Configures compiler behavior for treating warnings as errors.
#
# Features:
# - Adds `-Werror` to `CMAKE_C_FLAGS` if `ENABLE_WARN_AS_ERR` is ON
#
# Usage:
# - Enable with `-DENABLE_WARN_AS_ERR=ON` to enforce stricter builds
# ----------------------------------------------------------------------------

# Treat warnings as errors if ENABLE_WARN_AS_ERR is set
if(ENABLE_WARN_AS_ERR)
    message(STATUS "Treating warnings as errors (ENABLE_WARN_AS_ERR is ON)")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror" CACHE STRING "" FORCE)
endif()

# *** END OF FILE ***
