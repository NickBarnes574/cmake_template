# FILENAME: add_all_targets.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Adds multiple build targets, such as executables (EXE) and 
# shared libraries (SO), to a CMake project. It calls `add_exe()` or `add_so()`
# based on the specified target type.
# -----------------------------------------------------------------------------

function(add_all_targets)

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         demo_1_basic_pointer_usage
        ENDPOINT            LOCAL
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/pointers/demos/demo_1
        DESTINATION_DIR     pointers/demos
        LIBRARIES           Common
    )

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         demo_2_pointer_and_array
        ENDPOINT            LOCAL
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/pointers/demos/demo_2
        DESTINATION_DIR     pointers/demos
        LIBRARIES           Common
    )

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         demo_3_passing_pointers
        ENDPOINT            LOCAL
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/pointers/demos/demo_3
        DESTINATION_DIR     pointers/demos
        LIBRARIES           Common
    )

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         demo_4_dynamic_memory_allocation
        ENDPOINT            LOCAL
        TARGET_TYPE         EXE
        SOURCE_DIR          projects/pointers/demos/demo_4
        DESTINATION_DIR     pointers/demos
        LIBRARIES           Common
    )

endfunction()

# *** end of file ***