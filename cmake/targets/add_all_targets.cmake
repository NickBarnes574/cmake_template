# FILENAME: add_all_targets.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Adds multiple build targets, such as executables (EXE) and 
# shared libraries (SO), to a CMake project. It calls `add_exe()` or `add_so()`
# based on the specified target type.
# -----------------------------------------------------------------------------

function(add_all_targets)

    add_local_executables()
    add_remote_executables()
    add_pointer_demos()

endfunction()

# -----------------------------------------------------------------------------
# LOCAL EXECUTABLES
# -----------------------------------------------------------------------------

function(add_local_executables)

endfunction()

# -----------------------------------------------------------------------------
# REMOTE EXECUTABLES
# -----------------------------------------------------------------------------

function(add_remote_executables)

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         test_server
        ENDPOINT            remote
        TARGET_TYPE         EXE
        SOURCE_DIR          projects
        DESTINATION_DIR     projects
        LIBRARIES           Common Math DSA Threading Networking bcm2835
    )

endfunction()

# -----------------------------------------------------------------------------
# DEMOS
# -----------------------------------------------------------------------------

function(add_pointer_demos)

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         demo_1_basic_pointer_usage
        ENDPOINT            local
        TARGET_TYPE         EXE
        SOURCE_DIR          training/pointers/demos/1_basic_pointer_usage/
        DESTINATION_DIR     training/pointers/demos
        LIBRARIES           Common
    )

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         demo_2_pointer_and_array
        ENDPOINT            local
        TARGET_TYPE         EXE
        SOURCE_DIR          training/pointers/demos/2_pointer_and_array
        DESTINATION_DIR     training/pointers/demos
        LIBRARIES           Common
    )

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         demo_3_passing_pointers
        ENDPOINT            local
        TARGET_TYPE         EXE
        SOURCE_DIR          training/pointers/demos/3_passing_pointers_to_functions
        DESTINATION_DIR     training/pointers/demos
        LIBRARIES           Common
    )

    add_target(
    #   [ FIELD ]-----------[ VALUE ]
        TARGET_NAME         demo_4_dynamic_memory_allocation
        ENDPOINT            local
        TARGET_TYPE         EXE
        SOURCE_DIR          training/pointers/demos/4_dynamic_memory_allocation
        DESTINATION_DIR     training/pointers/demos
        LIBRARIES           Common
    )

endfunction()

# *** end of file ***