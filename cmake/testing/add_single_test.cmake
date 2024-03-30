# # add_single_test.cmake

# # Function to add a single test
# # Arguments:
# #   TEST_NAME: Name of the test (also used for the executable)
# #   SOURCE: Source file(s) for the test
# #   LIBRARIES: (optional) Libraries to link against, including the project's own library
# function(add_single_test TEST_NAME SOURCE)
#     find_package(CUnit REQUIRED)

#     add_executable(${TEST_NAME} ${SOURCE})
#     target_include_directories(${TEST_NAME} PRIVATE ${CUNIT_INCLUDE_DIRS})
#     target_link_libraries(${TEST_NAME} ${CUNIT_LIBRARIES} ${ARGN})

#     # Add the test to CTest
#     add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
# endfunction()
