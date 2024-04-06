# FILENAME: setup_testing.cmake
# Setup CUnit testing environment

# Find CUnit package
find_package(PkgConfig REQUIRED)
pkg_check_modules(CUNIT REQUIRED cunit)

# Updated function to add a CUnit test executable with "named" parameters
function(add_cunit_test)
    # Parse arguments
    set(options "")
    set(oneValueArgs TARGET_NAME PROJECT_DIRECTORY TEST_DIRECTORY OUTPUT_DIRECTORY)
    set(multiValueArgs ADDITIONAL_LIBRARIES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message(STATUS "TARGET_NAME: ${ARG_TARGET_NAME}")

    # Use parsed arguments
    set(LIB_NAME "${ARG_TARGET_NAME}_lib")
    file(GLOB SRC_FILES "${ARG_PROJECT_DIRECTORY}/src/*.c")
    set(INCLUDE_DIR ${ARG_PROJECT_DIRECTORY}/include)
    # Assuming create_library is a function you've defined elsewhere
    create_lib(${LIB_NAME} "${SRC_FILES}" ${INCLUDE_DIR})

    foreach(LIB IN LISTS ARG_ADDITIONAL_LIBRARIES)
        target_link_libraries(${LIB_NAME} ${LIB})
    endforeach()

    file(GLOB TEST_FILES "${ARG_TEST_DIRECTORY}/*.c")
    set(FULL_TARGET_NAME ${ARG_TARGET_NAME}_${CMAKE_SYSTEM_PROCESSOR})
    add_executable(${FULL_TARGET_NAME} ${TEST_FILES})
    target_link_libraries(${FULL_TARGET_NAME} ${CUNIT_LIBRARIES} ${LIB_NAME})
    target_include_directories(${FULL_TARGET_NAME} PRIVATE ${CUNIT_INCLUDE_DIRS})
    add_test(NAME ${FULL_TARGET_NAME} COMMAND ${FULL_TARGET_NAME})

    set_target_properties(${FULL_TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${ARG_OUTPUT_DIRECTORY}")
endfunction()
