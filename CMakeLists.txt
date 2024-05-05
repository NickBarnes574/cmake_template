# Minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Project Name
project(CMakeBuildSystem
    VERSION 1.0
    LANGUAGES C
)

enable_testing()

# Common utility functions
include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}")

include_cmake_modules("checks")     # Clang-Tidy checks
include_cmake_modules("options")    # Debug and Release options
include_cmake_modules("libraries")  # Custom user-made libraries
include_cmake_modules("targets")     # Target management
include_cmake_modules("install")    # Installation configuration
include_cmake_modules("testing")    # Test configuration

#Define _GNU_SOURCE for GNU extensions
add_definitions(-D_GNU_SOURCE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "*** building in DEBUG mode ***")
    set_default_debug_options()
else()
    message(STATUS "*** building in RELEASE mode ***")
    set_default_release_options()
endif()

add_all_libs()

add_all_targets()

add_all_tests()

install_all_files()

# *** end of file ***
