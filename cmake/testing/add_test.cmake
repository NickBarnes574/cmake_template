# FILENAME: add_test.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Adds a test to the test list
# -----------------------------------------------------------------------------

function(add_test)
    set(options "")
    set(oneValueArgs TEST_NAME SRC_FILES TEST_FILES)
    set(multiValueArgs LIBRARIES)
    cmake_parse_arguments(ADD_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message(STATUS "add_target() source directory: ${SOURCE_DIR}")
    set(SOURCE "${CMAKE_SOURCE_DIR}/${ADD_TEST_SOURCE_DIR}")

    message(STATUS "add_target() Libraries: ${ADD_TEST_LIBRARIES}")

    file(GLOB SOURCES "${SOURCE}/src/*.c")
    message(STATUS "add_target() message: ${SOURCES}")
    set(INCLUDES ${SOURCE}/local/include)

endfunction()

# ------------IN PROGRESS------------

# *** end of file ***