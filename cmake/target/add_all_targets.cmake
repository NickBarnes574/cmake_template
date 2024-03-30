# FILENAME: add_all_targets.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Adds multiple build targets, such as executables (EXE) and 
# shared libraries (SO), to a CMake project. It calls `add_exe()` or `add_so()`
# based on the specified target type.
# -----------------------------------------------------------------------------

function(add_all_targets)
    # add_target(hello_world LOCAL EXE projects/pointers/simple/hello_world pointers/simple Common Math)
    add_target(
        TARGET_NAME         ex_1
        ENDPOINT            LOCAL
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/pointers/simple/ex_1
        DESTINATION_DIR     pointers/simple
        LIBRARIES           Common
    )
    # add_target(hello_again LOCAL EXE projects/pointers/simple/hello_again pointers/simple Common Math)

    add_target(
        TARGET_NAME         ex_2
        ENDPOINT            LOCAL
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/pointers/simple/ex_2
        DESTINATION_DIR     pointers/simple
        LIBRARIES           Common
    )

endfunction()

# *** end of file ***