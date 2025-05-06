# ----------------------------------------------------------------------------
# BRIEF -- testing-config.cmake
# ----------------------------------------------------------------------------
# Sets up CUnit testing framework and related test configuration.
#
# Features:
# - Defines `add_cunit_test()` to register CUnit-based test executables
# - Enables `CTest` in `Test` build mode
#
# Usage:
# - Add test targets using `add_cunit_test()` macro
# - Enable test mode with `-DCMAKE_BUILD_TYPE=Test`
# ----------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/color-utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/project-config.cmake)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUNIT REQUIRED cunit)

# ----------------------------------------------------------------------------
# FUNCTION - add_cunit_test()
#
# Description:
# Defines a CUnit-based test target with sources, includes, and linked libraries.
#
# Usage:
# add_cunit_test(
#     <target_name>
#     SOURCES <file1> <file2> ...
#     DEPENDENCIES <lib1> <lib2> ...
#     INCLUDE_DIRS <dir1> <dir2> ...
# )
#
# Notes:
# - Automatically sets output directory via `configure_test_executable()`
# - Registers the test with CTest
# ----------------------------------------------------------------------------

function(add_cunit_test)
    cmake_parse_arguments(
        CUNIT
        ""                         # No options
        "TARGET;SCOPE"             # Single-value args
        "SOURCES;DEPENDENCIES;INCLUDE_DIRS"  # Multi-value args
        ${ARGN}
    )

    if(NOT CUNIT_TARGET)
        message(FATAL_ERROR "add_cunit_test: TARGET is required")
    endif()

    if(NOT CUNIT_SOURCES)
        message(FATAL_ERROR "add_cunit_test: SOURCES is required for target ${CUNIT_TARGET}")
    endif()

    add_executable(${CUNIT_TARGET} ${CUNIT_SOURCES})
    configure_test_executable(${CUNIT_TARGET} ${CUNIT_SCOPE})

    if(CUNIT_DEPENDENCIES)
        target_link_libraries(${CUNIT_TARGET} PRIVATE ${CUNIT_DEPENDENCIES})
    endif()
    target_link_libraries(${CUNIT_TARGET} PRIVATE cunit)

    if(CUNIT_INCLUDE_DIRS)
        target_include_directories(${CUNIT_TARGET} PRIVATE ${CUNIT_INCLUDE_DIRS})
    endif()

    add_test(NAME ${CUNIT_TARGET} COMMAND ${CUNIT_TARGET})
endfunction()

# *** END OF FILE ***
