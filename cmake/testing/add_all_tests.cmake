# FILENAME: add_all_tests.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to add test suites for each target in the 
# project. This script is responsible for integrating unit tests.
# -----------------------------------------------------------------------------

function(add_all_tests)

    add_cunit_test(
        TARGET_NAME             test_swap
        PROJECT_DIRECTORY       ${CMAKE_SOURCE_DIR}/projects/pointers/exercises/ex_1/local
        TEST_DIRECTORY          ${CMAKE_SOURCE_DIR}/projects/pointers/exercises/ex_1/local/tests
        OUTPUT_DIRECTORY        /pointers/exercises/ex_1
        ADDITIONAL_LIBRARIES    Common # Specify additional libraries as needed
    )

endfunction()

# *** end of file ***