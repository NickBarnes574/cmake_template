# cmake_minimum_required(VERSION 3.16)

# add_executable(test_adjacency_list
#     ${CMAKE_CURRENT_LIST_DIR}/adjacency_list_tests.c
# )

# target_link_libraries(test_adjacency_list cunit)
# INSTALL(TARGETS test_adjacency_list DESTINATION ${CMAKE_SOURCE_DIR}/build/)

# add_test(
#     NAME TestAdjacencyList_valgrind
#     COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes
# )