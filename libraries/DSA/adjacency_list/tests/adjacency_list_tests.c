#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "adjacency_list.h"
#include "comparisons.h"
#include "utilities.h"

void custom_free(void * data)
{
    (void)data;
    return;
}

void custom_print(const void * data)
{
    (void)data;
    printf("Node data\n");
}

void test_graph_create(void)
{
    graph_t * graph = graph_create(custom_free, int_comp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(graph->node_count, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->node_list);
    graph_destroy(&graph);
}

void test_graph_add_node(void)
{
    int       exit_code = E_FAILURE;
    graph_t * graph     = graph_create(custom_free, int_comp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);

    int data  = 1;
    exit_code = graph_add_node(graph, &data);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(graph->node_count, 1);

    graph_destroy(&graph);
}

void test_graph_remove_node(void)
{
    int       exit_code = E_FAILURE;
    graph_t * graph     = graph_create(custom_free, int_comp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);

    int data  = 1;
    exit_code = graph_add_node(graph, &data);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(graph->node_count, 1);

    exit_code = graph_remove_node(graph, &data);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(graph->node_count, 0);

    graph_destroy(&graph);
}

void test_graph_add_edge(void)
{
    int       exit_code = E_FAILURE;
    graph_t * graph     = graph_create(custom_free, int_comp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);

    int data1 = 1, data2 = 2;
    exit_code = graph_add_node(graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    graph_destroy(&graph);
}

void test_graph_remove_edge(void)
{
    int       exit_code = E_FAILURE;
    graph_t * graph     = graph_create(custom_free, int_comp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);

    int data1 = 1, data2 = 2;
    exit_code = graph_add_node(graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_add_edge(graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_remove_edge(graph, &data1, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    graph_destroy(&graph);
}

void test_graph_is_connected(void)
{
    int       exit_code = E_FAILURE;
    graph_t * graph     = graph_create(custom_free, int_comp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);

    int  data1 = 1, data2 = 2;
    bool is_connected = false;

    exit_code = graph_add_node(graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_is_connected(graph, &is_connected);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_FALSE(is_connected);

    exit_code = graph_add_edge(graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_is_connected(graph, &is_connected);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_TRUE(is_connected);

    graph_destroy(&graph);
}

void test_graph_is_cyclic(void)
{
    int       exit_code = E_FAILURE;
    graph_t * graph     = graph_create(custom_free, int_comp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);

    int  data1 = 1, data2 = 2;
    bool is_cyclic = false;

    exit_code = graph_add_node(graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_is_cyclic(graph, &is_cyclic);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_FALSE(is_cyclic);

    exit_code = graph_add_edge(graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_edge(graph, &data2, &data1, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_is_cyclic(graph, &is_cyclic);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_TRUE(is_cyclic);

    graph_destroy(&graph);
}

void test_graph_node_degree(void)
{
    int       exit_code = E_FAILURE;
    graph_t * graph     = graph_create(custom_free, int_comp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);

    int    data1 = 1, data2 = 2;
    size_t degree = 0;

    exit_code = graph_add_node(graph, &data1);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    exit_code = graph_add_node(graph, &data2);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_node_degree(graph, &data1, &degree);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(degree, 0);

    exit_code = graph_add_edge(graph, &data1, &data2, 10, false);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);

    exit_code = graph_node_degree(graph, &data1, &degree);
    CU_ASSERT_EQUAL(exit_code, E_SUCCESS);
    CU_ASSERT_EQUAL(degree, 1);

    graph_destroy(&graph);
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
    CU_TEST_INFO_NULL
};

CU_SuiteInfo adjacency_list_test_suite = {
    "Adjacency List Tests",
    NULL,                // Suite initialization function
    NULL,                // Suite cleanup function
    NULL,                // Suite setup function
    NULL,                // Suite teardown function
    adjacency_list_tests // The combined array of all tests
};

/*** end of file ***/
