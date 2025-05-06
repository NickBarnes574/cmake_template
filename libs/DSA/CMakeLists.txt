include(${CMAKE_SOURCE_DIR}/cmake/add-localized-lib.cmake)

add_localized_lib(
    TARGET      DSA
    TYPE        SHARED
    SOURCES
        adjacency_list/src/adjacency_list.c
        adjacency_matrix/src/adjacency_matrix.c
        hash_table/src/hash_table.c
        linked_list/src/linked_list.c
        queue/src/queue.c
        stack/src/stack.c
        vector/src/vector.c
    INCLUDES
        adjacency_list/include
        adjacency_matrix/include
        hash_table/include
        linked_list/include
        queue/include
        stack/include
        vector/include
)

# Link dependencies (Common and Math libraries)
target_link_libraries(DSA PUBLIC Core Math Strings Signals)

add_cunit_test(
    TARGET      adjacency_list_tests
    SCOPE       internal
    SOURCES
        adjacency_list/tests/adjacency_list_tests.c
        adjacency_list/tests/test_runner.c
    DEPENDENCIES
        DSA Core
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/adjacency_list/include
)

add_cunit_test(
    TARGET      linked_list_tests
    SCOPE       internal
    SOURCES
        linked_list/tests/linked_list_tests.c
        linked_list/tests/test_runner.c
    DEPENDENCIES
        DSA Core
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/linked_list/include
)

add_cunit_test(
    TARGET      queue_tests
    SCOPE       internal
    SOURCES
        queue/tests/queue_tests.c
        queue/tests/test_runner.c
    DEPENDENCIES
        DSA Core
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/queue/include
)

add_cunit_test(
    TARGET      stack_tests
    SCOPE       internal
    SOURCES
        stack/tests/stack_tests.c
        stack/tests/test_runner.c
    DEPENDENCIES
        DSA Core
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/stack/include
)
