# FILENAME: add_gtest.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Provides a function to integrate Google Test suites into the 
# project. It sets up test targets with their source files and includes.
# -----------------------------------------------------------------------------

# Function to add a Google Test suite
function(add_gtest target_name test_source_files)
    # Find GTest package
    find_package(GTest REQUIRED)

    # Add the test executable
    add_executable(${target_name}_gtest ${test_source_files})

    # Link with Google Test and the main target library
    target_link_libraries(${target_name}_gtest GTest::GTest GTest::Main ${target_name})

    # Add to CTest
    add_test(NAME ${target_name}_gtest COMMAND ${target_name}_gtest)
endfunction()

# *** end of file ***