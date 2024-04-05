# FILENAME: aarch64-glibc-toolchain.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: This CMake toolchain file is configured for cross-compiling
# using the aarch64 toolchain, specifically targeting Linux systems with
# the aarch64 architecture.
#
# NOTES: aarch64 toolchain can be downloaded at https://toolchains.bootlin.com.
# Install the toolchain by unzipping it, and placing it the /opt directory.
# -----------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(tools /opt/aarch64--glibc--stable-2024.02-1)
set(CMAKE_C_COMPILER ${tools}/bin/aarch64-linux-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/aarch64-linux-g++)
set(CMAKE_FIND_ROOT_PATH ${tools})
set(CMAKE_STRIP ${tools}/bin/aarch64-linux-strip)

# *** end of file ***