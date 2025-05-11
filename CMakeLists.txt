# ----------------------------------------------------------------------------
# BRIEF -- CMakeLists.txt for [My-Project]
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
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# CMake Project Setup
# ----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.31)

project(
  MY_PROJECT
  VERSION 1.0.0
  LANGUAGES C)

# C Standard
set(CMAKE_C_STANDARD          17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

# Configuration specific compile options
set(CMAKE_C_FLAGS_DEBUG "-g -Werror -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_TEST "-g")

# Feature Toggles
option(ENABLE_CLANG_TIDY  "Enable clang-tidy static analysis" ON)
option(ENABLE_WARN_AS_ERR "Treat all warnings as errors"      OFF)
option(ENABLE_ADSAN       "Enable Address-Sanitizer"          OFF)
option(STRIP_TARGET       "Strip all symbols from release"    ON)
option(BUILD_SHARED_LIBS  "Build dynamic library (.so) files" OFF)

enable_testing()

# ----------------------------------------------------------------------------
# Initialize CMake Modules
# ----------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    cmake_modules
    GIT_REPOSITORY  git@github.com:NickBarnes574/cmake-modules.git
    GIT_TAG         main
)

FetchContent_MakeAvailable(cmake_modules)

# Append the fetched modules directory to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${cmake_modules_SOURCE_DIR}/modules")

# IMPORTANT: `paths-config` and `project-config` must come first
include(paths-config)
include(project-config)

# Include other modules
include(add-app)
include(add-lib)
include(adsan-config)
include(build-type-config)
include(clang-tidy-config)
include(color-utils)
include(fetch-library)
include(output-config)
include(testing-config)
include(toolchain-config)
include(warnings-config)

# ----------------------------------------------------------------------------
# Initialize Output Directory
# ----------------------------------------------------------------------------
set_project_output_paths(MyProject)

# ----------------------------------------------------------------------------
# Begin Build Process
# ----------------------------------------------------------------------------
add_subdirectory(dependencies)
add_subdirectory(apps)

# *** END OF FILE ***
