# FILENAME: set_default_release_options.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Configures default compiler options for the release build 
# configuration. Optimizes for performance and omits debug information, also 
# defines a preprocessor macro to disable debug-specific sections of the code.
# -----------------------------------------------------------------------------

function(set_default_release_options)
    add_compile_options(-std=c17 -Wall -Wextra -pedantic)
    add_compile_definitions(NDEBUG)
endfunction()

# *** end of file ***