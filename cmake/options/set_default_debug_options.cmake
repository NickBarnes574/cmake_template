# FILENAME: set_default_debug_options.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Sets default compiler and linker options for the debug build 
# configuration. Allows selection between Address Sanitizer and Valgrind 
# friendly options.
# -----------------------------------------------------------------------------

# Adds an option to CMake to control the use of Address Sanitizer
option(USE_ADDRESS_SANITIZER "Use Address Sanitizer for Debug Builds" OFF)

function(set_default_debug_options)
    # Common compiler options
    add_compile_options(-std=c17 -Wall -Wextra -pedantic -Werror -g)
    
    # Check if Address Sanitizer should be used
    if(USE_ADDRESS_SANITIZER)
        # Add Address Sanitizer specific compile and link options
        add_compile_options(-fsanitize=address)
        link_libraries(-fsanitize=address)
    endif()

    # Define DEBUG preprocessor macro
    add_compile_definitions(DEBUG)
endfunction()

# *** end of file ***
