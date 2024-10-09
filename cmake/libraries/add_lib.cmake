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

    # Debugging message to check parsed directories
    message(STATUS "Adding library: ${LIB_NAME}")
    message(STATUS "LIB_DIRS: ${LIB_DIRS}")

    foreach(DIR IN LISTS LIB_DIRS)
        # Normalize the directory path
        get_filename_component(NORMALIZED_DIR "${DIR}" ABSOLUTE)
        
        # Debugging message to verify the directory being checked
        message(STATUS "Checking directory: ${NORMALIZED_DIR}")
        
        # Ensure directory structure is as expected
        message(STATUS "Checking if ${NORMALIZED_DIR}/src exists")
        if(NOT IS_DIRECTORY "${NORMALIZED_DIR}/src")
        message(FATAL_ERROR "Directory for ${LIB_NAME} does not have the expected 'src' structure: ${NORMALIZED_DIR}/src")
        endif()
        message(STATUS "Checking if ${NORMALIZED_DIR}/include exists")
        if(NOT IS_DIRECTORY "${NORMALIZED_DIR}/include")
            message(FATAL_ERROR "Directory for ${LIB_NAME} does not have the expected 'include' structure: ${NORMALIZED_DIR}/include")
        endif()

        # Glob for source files in each provided directory.
        file(GLOB_RECURSE TEMP_SOURCES "${NORMALIZED_DIR}/src/*.c")
        list(APPEND LIB_SOURCES ${TEMP_SOURCES})

        # Collect include directories
        list(APPEND LIB_INCLUDE_DIRS "${NORMALIZED_DIR}/include")
    endforeach()
    
    if(LIB_SOURCES)
        create_lib(${LIB_NAME} "${LIB_SOURCES}" "${LIB_INCLUDE_DIRS}")
    else()
        message(FATAL_ERROR "No source files found for library: ${LIB_NAME}")
    endif()
endfunction(add_lib)

# *** end of file ***
