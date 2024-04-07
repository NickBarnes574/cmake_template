#include "adjacency_matrix.h"
#include "string_operations.h"
#include "utilities.h"

struct node
{
    void *data;  // Data for the node
    char *label; // Node label
};

struct edge
{
    size_t weight; // Weight of the edge
};

struct matrix
{
    edge_t **edges;       // 2D array to represent the graph's edges
    node_t *nodes;        // Array of nodes
    size_t num_nodes;     // Number of nodes in the graph
    bool is_directed;     // Flag indicating whether the graph is directed
    CMP_F custom_compare; // A custom function to compare node data
    FREE_F custom_free;   // A custom function to free node data
};

static void graph_cleanup(matrix_t **graph)
{
    if (NULL == graph)
    {
        print_error("graph_cleanup(): NULL argument passed.");
        goto END;
    }

    // Free each edge
    for (size_t idx = 0; idx < (*graph)->num_nodes; idx++)
    {
        if (NULL != (*graph)->edges[idx])
        {
            free((*graph)->edges[idx]);
            (*graph)->edges[idx] = NULL;
        }
    }

    // Free the edge array
    free((*graph)->edges);
    (*graph)->edges = NULL;

    free((*graph)->nodes);
    (*graph)->nodes = NULL;

    free(*graph);
    (*graph) = NULL;

END:
    return;
}

matrix_t *matrix_init(size_t num_nodes,
                      bool is_directed,
                      CMP_F custom_compare,
                      FREE_F custom_free)
{
    matrix_t *graph = NULL;

    if ((NULL == custom_compare) || (NULL == custom_free))
    {
        print_error("matrix_init(): NULL argument passed.");
        goto END;
    }

    graph = calloc(1, sizeof(matrix_t));
    if (NULL == graph)
    {
        print_error("matrix_init(): graph - CMR failure.");
        graph_cleanup(&graph);
        goto END;
    }

    // Create an array to hold edges
    graph->edges = calloc(num_nodes, sizeof(edge_t *));
    if (NULL == graph->edges)
    {
        print_error("matrix_init(): graph->edges - CMR failure.");
        graph_cleanup(&graph);
        goto END;
    }

    // Allocate space for each initial edge
    for (size_t idx = 0; idx < num_nodes; idx++)
    {
        graph->edges[idx] = calloc(num_nodes, sizeof(edge_t));
        if (NULL == graph->edges[idx])
        {
            print_error("matrix_init(): graph->edges[idx] - CMR failure.");
            graph_cleanup(&graph);
            goto END;
        }
    }

    graph->nodes = calloc(num_nodes, sizeof(node_t));
    if (NULL == graph->nodes)
    {
        print_error("matrix_init(): graph->nodes - CMR failure.");
        graph_cleanup(&graph);
        goto END;
    }

    graph->num_nodes = num_nodes;
    graph->is_directed = is_directed;
    graph->custom_compare = custom_compare;
    graph->custom_free = custom_free;

END:
    return graph;
}

int matrix_populate_node(matrix_t *graph,
                         size_t index,
                         void *data,
                         char *label)
{
    int exit_code = E_FAILURE;
    char *label_copy = NULL;

    if ((NULL == graph) || (NULL == data) || (NULL == label))
    {
        print_error("matrix_populate_node(): NULL argument passed.");
        goto END;
    }

    // exit_code = copy_string(label, &label_copy, MAX_LABEL_SIZE, false);
    // if (E_SUCCESS != exit_code)
    // {
    //     print_error("matrix_populate_node(): Unable to copy label.");
    //     goto END;
    // }

    graph->nodes[index].data = data;
    graph->nodes[index].label = label_copy;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int matrix_depopulate_node(matrix_t *graph, size_t index)
{
    int exit_code = E_FAILURE;

    if (NULL == graph)
    {
        print_error("matrix_depopulate_node(): NULL argument passed.");
        goto END;
    }

    graph->custom_free(graph->nodes[index].data);
    graph->nodes[index].data = NULL;

    free(graph->nodes[index].label);
    graph->nodes[index].label = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

/*** end of file ***/
