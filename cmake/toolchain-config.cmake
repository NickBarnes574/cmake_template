# ----------------------------------------------------------------------------
# BRIEF -- toolchain-config.cmake
# ----------------------------------------------------------------------------
# Sets up the toolchain file for cross-compilation.
#
# Features:
# - Enables aarch64 toolchain file if `ARCH` is set to `aarch64`
#
# Usage:
# - Automatically activates if building for ARM 64-bit targets
# ----------------------------------------------------------------------------

# Set toolchain for cross-compilation (only if building for ARM)
if(ARCH STREQUAL "aarch64")
    set(AARCH64_TOOLCHAIN "${CMAKE_SOURCE_DIR}/cmake/toolchains/aarch64-glibc-toolchain.cmake")
    set(CMAKE_TOOLCHAIN_FILE ${AARCH64_TOOLCHAIN} CACHE STRING "Toolchain file" FORCE)
endif()

# *** END OF FILE ***
