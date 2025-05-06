# ----------------------------------------------------------------------------
# BRIEF -- CMakeLists.txt for Build System
# ----------------------------------------------------------------------------
# Top-level CMake configuration (C, version 1.0.0).
#
# Features:
# - C17 standard with strict compliance
# - Modular CMake structure for colorized output, static analysis, and build options
# - Feature toggles: static analysis, warnings as errors, AddressSanitizer
# - Supports dynamic/static library builds
#
# Usage:
# - Configure and build using the provided Makefile
# - Enable optional features using `-D<OPTION>=ON` flags
#
# TODO:
# - Add options for clang-format and cmake-format
# - Improve shared library installation and linking
# ----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.31)

project(
  BUILD_SYSTEM
  VERSION 1.0.0
  LANGUAGES C)

# C Standard
set(CMAKE_C_STANDARD          17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

# Create a library set of custom warnings that can be applied to multiple targets
add_library(Warnings INTERFACE)
target_compile_options(Warnings INTERFACE -Wall -Wextra -pedantic)

# Configuration specific compile options
set(CMAKE_C_FLAGS_DEBUG "-g -Werror -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_TEST "-g")

# Feature Toggles
option(COMPILE_EXECUTABLE "Compile the executable"            ON)
option(ENABLE_CLANG_TIDY  "Enable clang-tidy static analysis" ON)
option(ENABLE_WARN_AS_ERR "Treat all warnings as errors"      OFF)
option(ENABLE_ADSAN       "Enable Address-Sanitizer"          OFF)
option(STRIP_TARGET       "Strip all symbols from release"    ON)
option(BUILD_SHARED_LIBS  "Build dynamic library (.so) files" OFF)

enable_testing()

# Include CMake modules
include(cmake/adsan-config.cmake)
include(cmake/build-type-config.cmake)
include(cmake/clang-tidy-config.cmake)
include(cmake/output-config.cmake)
include(cmake/paths-config.cmake)
include(cmake/project-config.cmake)
include(cmake/testing-config.cmake)
include(cmake/toolchain-config.cmake)
include(cmake/warnings-config.cmake)

# Include project structure and libraries
add_subdirectory(apps)
add_subdirectory(libs)

# configure_executable()

message_color(STATUS "Building for ARCH: ${ARCH}, Mode: ${CMAKE_BUILD_TYPE}")

# *** END OF FILE ***
