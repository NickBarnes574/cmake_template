# FILENAME: create_library.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to create a static library with the specified 
# name, source files, and include directory. This script simplifies the library 
# creation process within the build system by abstracting common commands and 
# making them reusable for any given library within the project.
# -----------------------------------------------------------------------------

function(create_library LIB_NAME LIB_SOURCES LIB_INCLUDE_DIR)
    message(STATUS "Creating library: ${LIB_NAME}")
    message(STATUS "Library sources: ${LIB_SOURCES}")
    add_library(${LIB_NAME} STATIC ${LIB_SOURCES})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_INCLUDE_DIR})
endfunction(create_library)

# *** end of file ***
