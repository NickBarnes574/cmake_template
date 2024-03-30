# Minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Project Name
project(CMakeBuildSystem
    VERSION 1.0
    LANGUAGES C
)

# Utilities
include(${CMAKE_SOURCE_DIR}/cmake/utilities.cmake)

#Define _GNU_SOURCE for GNU extensions
add_definitions(-D_GNU_SOURCE)

include(cmake/buildsystem.cmake)

add_all_libraries()

# add_all_tests()

# *** end of file ***
