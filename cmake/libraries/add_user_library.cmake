# FILENAME: add_user_library.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to add a user-specified library to the project.
# This script dynamically identifies C source files within a specified library
# directory, sets the corresponding include directory, and leverages the
# create_library function to generate the library. It streamlines the inclusion
# of custom libraries into the build system.
# -----------------------------------------------------------------------------

function(add_user_library LIB_DIR)

    file(GLOB COMMON_LIB_SOURCES "${CMAKE_SOURCE_DIR}/libraries/${LIB_DIR}/src/*.c")
    set(COMMON_LIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libraries/${LIB_DIR}/include)

    create_library(${LIB_DIR} "${COMMON_LIB_SOURCES}" ${COMMON_LIB_INCLUDE_DIR})
endfunction(add_user_library)

# *** end of file ***
