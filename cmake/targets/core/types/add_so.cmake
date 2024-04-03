# FILENAME: add_so.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function, add_so, to add shared library targets to 
# the project.
# -----------------------------------------------------------------------------

function(add_so lib_name)
    add_library(${lib_name} SHARED ${ARGN})

    # Example of setting additional properties or custom configurations
    # You can modify or remove this part according to your needs
    set_target_properties(${lib_name} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
endfunction()

# *** end of file ***