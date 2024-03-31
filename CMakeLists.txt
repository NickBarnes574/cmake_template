# Minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Project Name
project(CMakeBuildSystem
    VERSION 1.0
    LANGUAGES C
)

enable_testing()

# Utilities
include(${CMAKE_SOURCE_DIR}/cmake/utilities.cmake)

include_cmake_modules("checks")     # Clang-Tidy checks
include_cmake_modules("options")    # Debug and Release options
include_cmake_modules("libraries")  # Custom user-made libraries
include_cmake_modules("target")     # Target management
include_cmake_modules("install")    # Installation configuration
include_cmake_modules("testing")    # Test configuration

#Define _GNU_SOURCE for GNU extensions
add_definitions(-D_GNU_SOURCE)

add_all_libraries()

add_all_targets()

add_all_tests()

# *** end of file ***
