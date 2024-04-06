#include "adjacency_list.h"
#include "utilities.h"

void edge_list_free(void * data_p)
{
    (void)data_p;
    return;
}

/**
 * @brief Collects all edges connected to a given node.
 *
 * This function is static and intended for internal use only.
 *
 * @param node_p A pointer to the node whose edges are to be collected.
 * @param num_edges A pointer to size_t to store the number of edges found.
 * @return edge_t** A pointer to an array of pointers to the collected edges.
 */
static edge_t ** collect_edges(node_t * node_p, size_t * num_edges);

/**
 * @brief Removes a set of edges from a graph.
 *
 * This function is static and intended for internal use only.
 *
 * @param graph_p A pointer to the graph from which edges are to be removed.
 * @param edges_to_remove_pp A pointer to an array of pointers to edges to be
 * removed.
 * @param num_edges The number of edges to remove.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
static int remove_edges(graph_t * graph_p,
                        edge_t ** edges_to_remove_pp,
                        size_t    num_edges);

/**
 * @brief Cleans up and frees a node from a graph.
 *
 * This function is static and intended for internal use only.
 *
 * @param graph_p A pointer to the graph containing the node.
 * @param node_p A pointer to the node to clean up.
 */
static void cleanup_node(graph_t * graph_p, node_t * node_p);

graph_t * graph_create(FREE_F custom_free, CMP_F custom_compare)
{
    graph_t * graph_p = NULL;

    if ((NULL == custom_free) || (NULL == custom_compare))
    {
        print_error("graph_create(): Missing custom free or compare function.");
        goto END;
    }

    graph_p = calloc(1, sizeof(graph_t));
    if (NULL == graph_p)
    {
        print_error("graph_create(): CMR failure.");
        goto END;
    }

    graph_p->node_count     = 0;
    graph_p->custom_free    = custom_free;
    graph_p->custom_compare = custom_compare;
    graph_p->node_list_p =
        list_new(graph_p->custom_free, graph_p->custom_compare);
    if (NULL == graph_p->node_list_p)
    {
        print_error("graph_add_node(): Unable to create node list.");
        free(graph_p);
        graph_p = NULL;
        goto END;
    }

END:
    return graph_p;
}

node_t * graph_create_node(void * data_p)
{
    node_t * new_node_p = NULL;

    if (NULL == data_p)
    {
        print_error("graph_create_node(): NULL data passed.");
        goto END;
    }

    new_node_p = calloc(1, sizeof(node_t));
    if (NULL == new_node_p)
    {
        print_error("graph_create_node(): CMR failure.");
        goto END;
    }

    new_node_p->data_p      = data_p;
    new_node_p->edge_count  = 0;
    new_node_p->edge_list_p = list_new(edge_list_free, NULL);
    if (NULL == new_node_p->edge_list_p)
    {
        print_error("graph_create_node(): CMR failure.");
        goto END;
    }

END:
    return new_node_p;
}

int graph_add_node(graph_t * graph_p, void * data_p)
{
    int      exit_code = E_FAILURE;
    node_t * node_p    = NULL;

    if ((NULL == graph_p) || (NULL == data_p))
    {
        print_error("graph_add_node(): NULL argument passed.");
        goto END;
    }

    node_p = graph_create_node(data_p);
    if (NULL == node_p)
    {
        print_error("graph_add_node(): Unable to create node.");
        goto END;
    }

    // Add the new node to the list
    exit_code = list_push_head(graph_p->node_list_p, node_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_add_node(): Unable to add new node to the list.");
        goto END;
    }

    graph_p->node_count += 1;

    exit_code = E_SUCCESS;
END:
    if (E_SUCCESS != exit_code)
    {
        free(node_p);
        node_p = NULL;
    }
    return exit_code;
}

int graph_remove_node(graph_t * graph_p, void * data_p)
{
    int       exit_code          = E_FAILURE;
    node_t *  node_p             = NULL;
    size_t    num_edges          = 0;
    edge_t ** edges_to_remove_pp = NULL;

    if ((NULL == graph_p) || (NULL == data_p))
    {
        print_error("graph_remove_node(): NULL argument passed.");
        goto END;
    }

    node_p = graph_find_node(graph_p, data_p);
    if (NULL == node_p)
    {
        print_error("graph_remove_node(): Unable to find node.");
        goto END;
    }

    edges_to_remove_pp = collect_edges(node_p, &num_edges);
    if (NULL == edges_to_remove_pp)
    {
        print_error("graph_remove_node(): Unable to collect edges.");
        goto END;
    }

    exit_code = remove_edges(graph_p, edges_to_remove_pp, num_edges);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_remove_node(): Unable to remove edges.");
        goto END;
    }

    // Remove the node from the list of nodes
    exit_code = list_remove_data(graph_p->node_list_p, node_p->data_p);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "graph_remove_node(): Unable to remove node from node list.");
        goto END;
    }

    graph_p->node_count -= 1;

    cleanup_node(graph_p, node_p);

    exit_code = E_SUCCESS;
END:
    free(edges_to_remove_pp);
    edges_to_remove_pp = NULL;
    return exit_code;
}

int graph_add_edge(graph_t * graph_p,
                   void *    data_1_p,
                   void *    data_2_p,
                   size_t    weight,
                   bool      is_bidirectional)
{
    int      exit_code = E_FAILURE;
    node_t * node_1_p  = NULL;
    node_t * node_2_p  = NULL;
    edge_t * edge_p    = NULL;

    if ((NULL == graph_p) || (NULL == data_1_p) || (NULL == data_2_p))
    {
        print_error("graph_add_edge(): NULL argument passed.");
        goto END;
    }

    node_1_p = graph_find_node(graph_p, data_1_p);
    if (NULL == node_1_p)
    {
        print_error("graph_add_edge(): Node 1 does not exist.");
        goto END;
    }

    node_2_p = graph_find_node(graph_p, data_2_p);
    if (NULL == node_2_p)
    {
        print_error("graph_add_edge(): Node 2 does not exist.");
        goto END;
    }

    edge_p = calloc(1, sizeof(edge_t));
    if (NULL == edge_p)
    {
        print_error("graph_add_edge(): CMR failure.");
        goto END;
    }

    edge_p->node_1_p    = node_1_p;
    edge_p->node_2_p    = node_2_p;
    edge_p->weight      = weight;
    edge_p->is_directed = !is_bidirectional;

    exit_code = list_push_head(node_1_p->edge_list_p, edge_p);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "graph_add_edge(): Unable to add edge to node 1's edge list.");
        goto END;
    }
    node_1_p->edge_count += 1;

    if (true == is_bidirectional)
    {
        exit_code = list_push_head(node_2_p->edge_list_p, edge_p);
        if (E_SUCCESS != exit_code)
        {
            print_error(
                "graph_add_edge(): Unable to add edge to node 2's edge list.");
            exit_code = list_remove_data(node_1_p->edge_list_p, edge_p);
            if (E_SUCCESS != exit_code)
            {
                print_error(
                    "graph_add_edge(): Unable to rollback previous add.");
            }
            node_1_p->edge_count -= 1;
            exit_code = E_FAILURE;
            goto END;
        }
        node_2_p->edge_count += 1;
    }

    exit_code = E_SUCCESS;
END:
    if (E_SUCCESS != exit_code)
    {
        free(edge_p);
        edge_p = NULL;
    }
    return exit_code;
}

int graph_remove_edge(graph_t * graph_p, void * data_1_p, void * data_2_p)
{
    int      exit_code = E_FAILURE;
    node_t * node_1_p  = NULL;
    node_t * node_2_p  = NULL;
    edge_t * edge_p    = NULL;

    if ((NULL == graph_p) || (NULL == data_1_p) || (NULL == data_2_p))
    {
        print_error("graph_remove_edge(): NULL argument passed.");
        goto END;
    }

    node_1_p = graph_find_node(graph_p, data_1_p);
    if (NULL == node_1_p)
    {
        print_error("graph_remove_edge(): Unable to find node 1.");
        goto END;
    }

    node_2_p = graph_find_node(graph_p, data_2_p);
    if (NULL == node_2_p)
    {
        print_error("graph_remove_edge(): Unable to find node 2.");
        goto END;
    }

    edge_p = graph_find_edge(graph_p, node_1_p, node_2_p);
    if (NULL == edge_p)
    {
        print_error("graph_remove_edge(): Unable to find edge.");
        goto END;
    }

    // Remove the edge
    exit_code = list_remove_data(node_1_p->edge_list_p, edge_p);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "graph_remove_edge(): Unable to remove edge from node 1's edge "
            "list.");
        goto END;
    }
    node_1_p->edge_count -= 1;

    // Remove the edge from node 2's list if non-directed
    if (false == edge_p->is_directed)
    {
        exit_code = list_remove_data(node_2_p->edge_list_p, edge_p);
        if (E_SUCCESS != exit_code)
        {
            print_error(
                "graph_remove_edge(): Unable to remove edge from node 2's edge "
                "list.");
            goto END;
        }
        node_2_p->edge_count -= 1;
    }

    free(edge_p);
    edge_p = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}
size_t graph_get_size(graph_t * graph_p)
{
    size_t size = 0;

    if (NULL == graph_p)
    {
        print_error("graph_get_size(): NULL argument passed.");
        goto END;
    }

    size = graph_p->node_count;

END:
    return size;
}

node_t * graph_find_node(graph_t * graph_p, void * data_p)
{
    list_node_t * list_node_p = NULL;
    node_t *      node_p      = NULL;

    if ((NULL == graph_p) || (NULL == data_p))
    {
        print_error("graph_find_node(): NULL argument passed.");
        goto END;
    }

    list_node_p = list_find_first_occurrence(graph_p->node_list_p, &data_p);
    if (NULL == list_node_p)
    {
        print_error("graph_find_node(): Unable to find first occurrence.");
    }

    node_p = (node_t *)list_node_p->data;

END:
    return node_p;
}
edge_t * graph_find_edge(graph_t * graph_p,
                         node_t *  node_1_p,
                         node_t *  node_2_p)
{
    edge_t *      edge_p      = NULL;
    list_t *      edge_list_p = NULL;
    list_node_t * current_p   = NULL;

    if ((NULL == graph_p) || (NULL == node_1_p) || (NULL == node_2_p))
    {
        print_error("graph_find_edge(): NULL argument passed.");
        goto END;
    }

    edge_list_p = node_1_p->edge_list_p;
    if (NULL == edge_list_p)
    {
        print_error("graph_find_edge(): Node 1 does not have an edge list.");
        goto END;
    }

    // Find the edge that connects to node 2
    current_p = edge_list_p->head;
    while (NULL != current_p)
    {
        edge_p = (edge_t *)current_p->data;

        // Check if the current edge connects to node 2
        if (((edge_p->node_1_p == node_1_p) &&
             (edge_p->node_2_p == node_2_p)) ||
            ((edge_p->node_1_p == node_2_p) && (edge_p->node_2_p == node_1_p) &&
             false == edge_p->is_directed))
        {
            // Found the edge
            goto END;
        }

        current_p = current_p->next;
    }

END:
    return edge_p;
}

void graph_print(graph_t * graph_p, PRINT_F custom_print)
{
    list_node_t * current_p    = NULL;
    node_t *      graph_node_p = NULL;

    if ((NULL == graph_p) || (NULL == custom_print))
    {
        print_error("graph_print(): NULL argument passed.");
        goto END;
    }

    current_p = graph_p->node_list_p->head;
    while (NULL != current_p)
    {
        graph_node_p = (node_t *)(current_p->data);
        printf("Node: ");
        custom_print(graph_node_p->data_p);
        printf(" with %zu edges.\n", graph_node_p->edge_count);

        current_p = current_p->next;
    }

END:
    return;
}

int graph_clear(graph_t * graph_p)
{
    int           exit_code = E_FAILURE;
    list_node_t * current_p = NULL;
    list_node_t * next_p    = NULL;
    node_t *      node_p    = NULL;

    if (NULL == graph_p)
    {
        print_error("graph_clear(): NULL argument passed.");
        goto END;
    }

    current_p = graph_p->node_list_p->head;
    while (NULL != current_p)
    {
        node_p = (node_t *)current_p->data;
        next_p = current_p->next;

        cleanup_node(graph_p, node_p);

        current_p = next_p;
    }

    exit_code = list_delete(&(graph_p->node_list_p));
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_clear(): Unable to reset graph's node list.");
        goto END;
    }

    graph_p->node_list_p = NULL;
    graph_p->node_count  = 0;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int graph_destroy(graph_t ** graph_pp)
{
    int exit_code = E_FAILURE;

    if ((graph_pp == NULL) || (*graph_pp == NULL))
    {
        print_error("graph_destroy(): NULL graph pointer.");
        goto END;
    }

    exit_code = graph_clear(*graph_pp);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_destroy(): Unable to clear graph.");
        goto END;
    }

    // Finally, free the graph structure
    free(*graph_pp);
    *graph_pp = NULL; // Avoid dangling pointer

END:
    return exit_code;
}

// STATIC FUNCTIONS

static edge_t ** collect_edges(node_t * node_p, size_t * num_edges)
{
    edge_t **     edge_list_pp = NULL;
    list_node_t * current_p    = NULL;
    size_t        index        = 0;

    if ((NULL == node_p) || (NULL == num_edges))
    {
        print_error("collect_edges(): NULL argument passed.");
        goto END;
    }

    *num_edges = node_p->edge_list_p->size;

    edge_list_pp = calloc(*num_edges, sizeof(edge_t *));
    if (NULL == edge_list_pp)
    {
        print_error("collect_edges(): CMR failure.");
        goto END;
    }

    current_p = node_p->edge_list_p->head;
    while ((NULL != current_p) && (index < *num_edges))
    {
        edge_list_pp[index++] = (edge_t *)current_p->data;
        current_p             = current_p->next;
    }

END:
    return edge_list_pp;
}

static int remove_edges(graph_t * graph_p,
                        edge_t ** edges_to_remove_pp,
                        size_t    num_edges)
{
    int exit_code = E_FAILURE;

    if ((NULL == graph_p) || (NULL == edges_to_remove_pp))
    {
        print_error("remove_edges(): NULL argument passed.");
        goto END;
    }

    for (size_t idx = 0; idx < num_edges; ++idx)
    {
        exit_code =
            graph_remove_edge(graph_p,
                              edges_to_remove_pp[idx]->node_1_p->data_p,
                              edges_to_remove_pp[idx]->node_2_p->data_p);
        if (E_SUCCESS != exit_code)
        {
            print_error("remove_edges(): Unable to remove edge.");
            goto END;
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void cleanup_node(graph_t * graph_p, node_t * node_p)
{
    int exit_code = E_FAILURE;

    if ((NULL == graph_p) || (NULL == node_p))
    {
        print_error("cleanup_node(): NULL argument passed.");
        goto END;
    }

    // Free the nodes data and the node itself, and remove the edge list
    graph_p->custom_free(node_p->data_p);
    node_p->data_p     = NULL;
    node_p->edge_count = 0;

    free(node_p->edge_list_p);
    node_p->edge_list_p = NULL;

    free(node_p);
    node_p = NULL;

    (void)exit_code;
END:
    return;
}

/*** end of file ***/
