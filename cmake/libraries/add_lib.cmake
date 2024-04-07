# FILENAME: add_lib.cmake
# -----------------------------------------------------------------------------
# Function to add a user-specified library to the project, automatically
# identifying C source files and include directories based on provided directories.
# `DIRS` is expected to be a list of directories where each directory contains
# a `src` folder for source files and an `include` folder for headers.
# -----------------------------------------------------------------------------

function(add_lib LIB_NAME)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs DIRS)
    cmake_parse_arguments(LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(DIR IN LISTS LIB_DIRS)
        # Ensure directory structure is as expected
        if(NOT EXISTS "${DIR}/src" OR NOT EXISTS "${DIR}/include")
            message(FATAL_ERROR "Directory for ${LIB_NAME} does not have the expected 'src' or 'include' structure: ${DIR}")
        endif()

        # Glob for source files in each provided directory.
        file(GLOB_RECURSE TEMP_SOURCES "${DIR}/src/*.c")
        list(APPEND LIB_SOURCES ${TEMP_SOURCES})

        # Collect include directories
        list(APPEND LIB_INCLUDE_DIRS "${DIR}/include")
    endforeach()
    
    if(LIB_SOURCES)
        create_lib(${LIB_NAME} "${LIB_SOURCES}" "${LIB_INCLUDE_DIRS}")
    else()
        message(FATAL_ERROR "No source files found for library: ${LIB_NAME}")
    endif()
endfunction(add_lib)

# *** end of file ***
