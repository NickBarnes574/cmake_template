# FILENAME: set_default_debug_options.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Sets default compiler and linker options for the debug build 
# configuration. It includes compiler flags for stricter warnings, address 
# sanitization, and a preprocessor macro for enabling debug-specific code 
# sections.
# -----------------------------------------------------------------------------

# function(set_default_debug_options)
#     add_compile_options(-std=c17 -Wall -Wextra -pedantic -Werror -fsanitize=address -g)
#     link_libraries(-fsanitize=address)
#     add_compile_definitions(DEBUG)
# endfunction()

function(set_default_debug_options)
    add_compile_options(-std=c17 -Wall -Wextra -pedantic -Werror -g)
    add_compile_definitions(DEBUG)
endfunction()

# *** end of file ***
