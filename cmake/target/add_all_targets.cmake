# FILENAME: add_all_targets.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Adds multiple build targets, such as executables (EXE) and 
# shared libraries (SO), to a CMake project. It calls `add_exe()` or `add_so()`
# based on the specified target type.
# -----------------------------------------------------------------------------

function(add_all_targets)

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         bsle_server
        ENDPOINT            REMOTE
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/BSLE
        DESTINATION_DIR     projects/BSLE
        LIBRARIES           # No custom libraries added
    )

endfunction()

# *** end of file ***