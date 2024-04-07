# FILENAME: create_lib.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to create a static library with the specified 
# name, source files, and include directory. This script simplifies the library 
# creation process within the build system by abstracting common commands and 
# making them reusable for any given library within the project.
# -----------------------------------------------------------------------------

function(create_lib LIB_NAME LIB_SOURCES LIB_INCLUDE_DIRS)
    message(STATUS "Creating library: ${LIB_NAME}")
    add_library(${LIB_NAME} STATIC ${LIB_SOURCES})
    
    # Include all specified directories
    foreach(INCLUDE_DIR IN LISTS LIB_INCLUDE_DIRS)
        target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
    endforeach()
endfunction(create_lib)

# *** end of file ***
