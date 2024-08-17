#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "adjacency_list.h"
#include "comparisons.h"
#include "utilities.h"

graph_t * test_graph = NULL;

void custom_free(void * data)
{
    (void)data;
    return;
}

void custom_print(const void * data)
{
    int * int_data = (int *)data;
    printf("%d ", *int_data);
}

void setup(void)
{
    test_graph = graph_create(custom_free, int_comp);
}

void teardown(void)
{
    if (NULL != test_graph)
    {
        graph_destroy(&test_graph);
    }
}

void test_graph_create(void)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(test_graph);
    CU_ASSERT_EQUAL(test_graph->node_count, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(test_graph->node_list);
}

void test_graph_add_node(void)
{
    int exit_code = E_FAILURE;
    int data      = 1;

    exit_code = graph_add_node(test_graph, &data);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(test_graph->node_count, 1);
}

void test_graph_remove_node(void)
{
    int exit_code = E_FAILURE;
    int data      = 1;

    exit_code = graph_add_node(test_graph, &data);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(test_graph->node_count, 1);

    exit_code = graph_remove_node(test_graph, &data);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(test_graph->node_count, 0);
}

void test_graph_add_edge(void)
{
    int exit_code = E_FAILURE;
    int data1 = 1, data2 = 2;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

void test_graph_remove_edge(void)
{
    int exit_code = E_FAILURE;
    int data1 = 1, data2 = 2;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_remove_edge(test_graph, &data1, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

void test_graph_is_connected(void)
{
    int  exit_code = E_FAILURE;
    int  data1 = 1, data2 = 2;
    bool is_connected = false;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_is_connected(test_graph, &is_connected);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_FALSE(is_connected);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_is_connected(test_graph, &is_connected);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_TRUE(is_connected);
}

void test_graph_is_cyclic(void)
{
    int  exit_code = E_FAILURE;
    int  data1 = 1, data2 = 2;
    bool is_cyclic = false;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_is_cyclic(test_graph, &is_cyclic);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_FALSE(is_cyclic);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_edge(test_graph, &data2, &data1, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_is_cyclic(test_graph, &is_cyclic);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_TRUE(is_cyclic);
}

void test_graph_node_degree(void)
{
    int    exit_code = E_FAILURE;
    int    data1 = 1, data2 = 2;
    size_t degree = 0;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_node_degree(test_graph, &data1, &degree);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(degree, 0);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_node_degree(test_graph, &data1, &degree);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(degree, 1);
}

void test_graph_get_adjacent_nodes(void)
{
    int exit_code = E_FAILURE;
    int data1 = 1, data2 = 2;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    list_t * adjacent_nodes = graph_get_adjacent_nodes(test_graph, &data1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(adjacent_nodes);
    CU_ASSERT_EQUAL(adjacent_nodes->size, 1);
    list_delete(&adjacent_nodes);
}

void test_graph_edge_exists(void)
{
    int exit_code = E_FAILURE;
    int data1 = 1, data2 = 2;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_TRUE(graph_edge_exists(test_graph, &data1, &data2));
    CU_ASSERT_FALSE(graph_edge_exists(test_graph, &data2, &data1));
}

void test_graph_get_edge_weight(void)
{
    int exit_code = E_FAILURE;
    int data1 = 1, data2 = 2;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    CU_ASSERT_EQUAL(graph_get_edge_weight(test_graph, &data1, &data2), 10);
}

void test_graph_clone(void)
{
    int exit_code = E_FAILURE;
    int data1 = 1, data2 = 2;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    graph_t * clone = graph_clone(test_graph);
    CU_ASSERT_PTR_NOT_NULL_FATAL(clone);
    CU_ASSERT_EQUAL(clone->node_count, 2);
    CU_ASSERT_TRUE(graph_edge_exists(clone, &data1, &data2));
    graph_destroy(&clone);
}

void test_graph_dfs(void)
{
    int exit_code = E_FAILURE;
    int data1 = 1, data2 = 2;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_dfs(test_graph, &data1, custom_print);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

void test_graph_bfs(void)
{
    int exit_code = E_FAILURE;
    int data1 = 1, data2 = 2;

    exit_code = graph_add_node(test_graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(test_graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(test_graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_bfs(test_graph, &data1, custom_print);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
}

static CU_TestInfo adjacency_list_tests[] = {
    { "graph_create", test_graph_create },
    { "graph_add_node", test_graph_add_node },
    { "graph_remove_node", test_graph_remove_node },
    { "graph_add_edge", test_graph_add_edge },
    { "graph_remove_edge", test_graph_remove_edge },
    { "graph_is_connected", test_graph_is_connected },
    { "graph_is_cyclic", test_graph_is_cyclic },
    { "graph_node_degree", test_graph_node_degree },
    { "graph_get_adjacent_nodes", test_graph_get_adjacent_nodes },
    { "graph_edge_exists", test_graph_edge_exists },
    { "graph_get_edge_weight", test_graph_get_edge_weight },
    { "graph_clone", test_graph_clone },
    { "graph_dfs", test_graph_dfs },
    { "graph_bfs", test_graph_bfs },
    CU_TEST_INFO_NULL
};

CU_SuiteInfo adjacency_list_test_suite = {
    "Adjacency List Tests",
    NULL,                // Suite initialization function
    NULL,                // Suite cleanup function
    setup,               // Suite setup function
    teardown,            // Suite teardown function
    adjacency_list_tests // The combined array of all tests
};

/*** end of file ***/
