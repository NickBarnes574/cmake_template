#   LIB_NAME: The name of the library to be created.
#   DIR_SUFFIX: The suffix of the directory path where the library's source and include files are located.
function(setup_library LIB_NAME DIR_SUFFIX)
    # Set the directory for the library's source and include files based on the provided DIR_SUFFIX
    set(LIB_DIR ${CMAKE_SOURCE_DIR}/libraries/${DIR_SUFFIX})

    # Automatically gather all C source files in the library's src directory
    file(GLOB LIB_SOURCES "${LIB_DIR}/src/*.c")

    # Set the path for the library's include directory
    set(LIB_INCLUDES "${LIB_DIR}/include")

    # Add a shared library with the gathered source files
    add_library(${LIB_NAME} STATIC ${LIB_SOURCES})

    # Specify the public include directories for the library
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_INCLUDES})
endfunction()

# *** end of file ***
