# FILENAME: add_all_tests.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to add test suites for each target in the 
# project. This script is responsible for integrating unit tests.
# -----------------------------------------------------------------------------

function(add_all_tests)

    library_tests()
    training_tests()

endfunction()

function(library_tests)

    add_cunit_test(
        TARGET_NAME             test_stack
        PROJECT_DIRECTORY       ${CMAKE_SOURCE_DIR}/libraries/DSA/stack
        TEST_DIRECTORY          ${CMAKE_SOURCE_DIR}/libraries/DSA/stack/tests
        OUTPUT_DIRECTORY        ${CMAKE_SYSTEM_PROCESSOR}/tests/library_tests/${CMAKE_BUILD_TYPE}/DSA
        ADDITIONAL_LIBRARIES    Common # Specify additional libraries as needed
    )

    add_cunit_test(
        TARGET_NAME             test_queue
        PROJECT_DIRECTORY       ${CMAKE_SOURCE_DIR}/libraries/DSA/queue
        TEST_DIRECTORY          ${CMAKE_SOURCE_DIR}/libraries/DSA/queue/tests
        OUTPUT_DIRECTORY        ${CMAKE_SYSTEM_PROCESSOR}/tests/library_tests/${CMAKE_BUILD_TYPE}/DSA
        ADDITIONAL_LIBRARIES    Common # Specify additional libraries as needed
    )

    add_cunit_test(
        TARGET_NAME             test_linked_list
        PROJECT_DIRECTORY       ${CMAKE_SOURCE_DIR}/libraries/DSA/linked_list
        TEST_DIRECTORY          ${CMAKE_SOURCE_DIR}/libraries/DSA/linked_list/tests
        OUTPUT_DIRECTORY        ${CMAKE_SYSTEM_PROCESSOR}/tests/library_tests/${CMAKE_BUILD_TYPE}/DSA
        ADDITIONAL_LIBRARIES    Common Math# Specify additional libraries as needed
    )

    add_cunit_test(
        TARGET_NAME             test_adjacency_list
        PROJECT_DIRECTORY       ${CMAKE_SOURCE_DIR}/libraries/DSA/adjacency_list
        TEST_DIRECTORY          ${CMAKE_SOURCE_DIR}/libraries/DSA/adjacency_list/tests
        OUTPUT_DIRECTORY        ${CMAKE_SYSTEM_PROCESSOR}/tests/library_tests/${CMAKE_BUILD_TYPE}/DSA
        ADDITIONAL_LIBRARIES    Common Math DSA# Specify additional libraries as needed
    )

endfunction()

function(training_tests)

    add_cunit_test(
        TARGET_NAME             test_swap
        PROJECT_DIRECTORY       ${CMAKE_SOURCE_DIR}/training/pointers/exercises/1_swap/local
        TEST_DIRECTORY          ${CMAKE_SOURCE_DIR}/training/pointers/exercises/1_swap/local/tests
        OUTPUT_DIRECTORY        ${CMAKE_SYSTEM_PROCESSOR}tests/training_tests/pointers/exercises/1_swap
        ADDITIONAL_LIBRARIES    Common # Specify additional libraries as needed
    )

    add_cunit_test(
        TARGET_NAME             test_reverse_array
        PROJECT_DIRECTORY       ${CMAKE_SOURCE_DIR}/training/pointers/exercises/2_reverse_array/local
        TEST_DIRECTORY          ${CMAKE_SOURCE_DIR}/training/pointers/exercises/2_reverse_array/local/tests
        OUTPUT_DIRECTORY        ${CMAKE_SYSTEM_PROCESSOR}tests/training_tests/pointers/exercises/2_reverse_array
        ADDITIONAL_LIBRARIES    Common # Specify additional libraries as needed
    )

endfunction()

# *** end of file ***