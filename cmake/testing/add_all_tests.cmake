# # FILENAME: add_all_tests.cmake
# # -----------------------------------------------------------------------------
# # DESCRIPTION: Defines a function to add test suites for each target in the 
# # project. This script is responsible for integrating unit tests.
# # -----------------------------------------------------------------------------

# Function to add test suites for each target
function(add_all_tests)

# set(ADJACENCY_LIST_TEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/libraries/2_DataStructures/tests/adjacency_list_tests.c")
# add_single_test(test_adjacency_list ${ADJACENCY_LIST_TEST_SOURCE})

add_subdirectory(PATH)
install_target(test_name test_output_dir)

endfunction()

# # *** end of file ***