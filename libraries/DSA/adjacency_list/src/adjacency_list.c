#include "adjacency_list.h"
#include "queue.h"
#include "stack.h"
#include "utilities.h"

#define MAX_STACK_SIZE 2000

void edge_list_free(void * data)
{
    (void)data;
    return;
}

comp_rtns_t edge_compare(void * a, void * b)
{
    const edge_t * edge_a = (const edge_t *)a;
    const edge_t * edge_b = (const edge_t *)b;

    if (edge_a->node_1 == edge_b->node_1 && edge_a->node_2 == edge_b->node_2)
    {
        return EQUAL;
    }
    else if (edge_a->node_1 == edge_b->node_2 &&
             edge_a->node_2 == edge_b->node_1 && !edge_a->is_directed)
    {
        return EQUAL;
    }

    return NOT_EQUAL;
}

/**
 * @brief Creates a new node with the given data.
 *
 * @param data A pointer to the data to be stored in the node.
 * @return node_t* A pointer to the newly created node or NULL on failure.
 */
static node_t * graph_create_node(void * data);

/**
 * @brief Finds a node in the graph with the given data.
 *
 * @param graph A pointer to the graph.
 * @param data A pointer to the data of the node to find.
 * @return node_t* A pointer to the found node or NULL if not found.
 */
static node_t * graph_find_node(graph_t * graph, void * data);

/**
 * @brief Finds an edge between two nodes in the graph.
 *
 * @param graph A pointer to the graph.
 * @param node_1 A pointer to the first node.
 * @param node_2 A pointer to the second node.
 * @return edge_t* A pointer to the found edge or NULL if not found.
 */
static edge_t * graph_find_edge(graph_t * graph,
                                node_t *  node_1,
                                node_t *  node_2);

/**
 * @brief Collects all edges connected to a given node.
 *
 * This function is static and intended for internal use only.
 *
 * @param node A pointer to the node whose edges are to be collected.
 * @param num_edges A pointer to size_t to store the number of edges found.
 * @return edge_t** A pointer to an array of pointers to the collected edges.
 */
static edge_t ** collect_edges(node_t * node, uint32_t * num_edges);

/**
 * @brief Removes a set of edges from a graph.
 *
 * This function is static and intended for internal use only.
 *
 * @param graph A pointer to the graph from which edges are to be removed.
 * @param edges_to_remove A pointer to an array of pointers to edges to be
 * removed.
 * @param num_edges The number of edges to remove.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
static int remove_edges(graph_t * graph,
                        edge_t ** edges_to_remove,
                        size_t    num_edges);

/**
 * @brief Cleans up and frees a node from a graph.
 *
 * This function is static and intended for internal use only.
 *
 * @param graph A pointer to the graph containing the node.
 * @param node A pointer to the node to clean up.
 */
static void cleanup_node(graph_t * graph, node_t * node);

graph_t * graph_create(FREE_F custom_free, CMP_F custom_compare)
{
    graph_t * graph = NULL;

    if ((NULL == custom_free) || (NULL == custom_compare))
    {
        print_error("graph_create(): Missing custom free or compare function.");
        goto END;
    }

    graph = calloc(1, sizeof(graph_t));
    if (NULL == graph)
    {
        print_error("graph_create(): CMR failure.");
        goto END;
    }

    graph->node_count     = 0;
    graph->custom_free    = custom_free;
    graph->custom_compare = custom_compare;
    graph->node_list      = list_new(graph->custom_free, graph->custom_compare);
    if (NULL == graph->node_list)
    {
        print_error("graph_add_node(): Unable to create node list.");
        free(graph);
        graph = NULL;
        goto END;
    }

END:
    return graph;
}

int graph_add_node(graph_t * graph, void * data)
{
    int      exit_code = E_FAILURE;
    node_t * node      = NULL;

    if ((NULL == graph) || (NULL == data))
    {
        print_error("graph_add_node(): NULL argument passed.");
        goto END;
    }

    node = graph_create_node(data);
    if (NULL == node)
    {
        print_error("graph_add_node(): Unable to create node.");
        goto END;
    }

    // Add the new node to the list
    exit_code = list_push_head(graph->node_list, node);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_add_node(): Unable to add new node to the list.");
        goto END;
    }

    graph->node_count += 1;

    exit_code = E_SUCCESS;
END:
    if (E_SUCCESS != exit_code)
    {
        free(node);
        node = NULL;
    }
    return exit_code;
}

int graph_remove_node(graph_t * graph, void * data)
{
    int       exit_code       = E_FAILURE;
    node_t *  node            = NULL;
    uint32_t  num_edges       = 0;
    edge_t ** edges_to_remove = NULL;

    if ((NULL == graph) || (NULL == data))
    {
        print_error("graph_remove_node(): NULL argument passed.");
        goto END;
    }

    node = graph_find_node(graph, data);
    if (NULL == node)
    {
        print_error("graph_remove_node(): Unable to find node.");
        goto END;
    }

    edges_to_remove = collect_edges(node, &num_edges);
    if (NULL == edges_to_remove)
    {
        print_error("graph_remove_node(): Unable to collect edges.");
        goto END;
    }

    exit_code = remove_edges(graph, edges_to_remove, num_edges);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_remove_node(): Unable to remove edges.");
        goto END;
    }

    // Remove the node from the list of nodes
    exit_code = list_remove_data(graph->node_list, &(node->data));
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "graph_remove_node(): Unable to remove node from node list.");
        goto END;
    }

    graph->node_count -= 1;

    cleanup_node(graph, node);

    exit_code = E_SUCCESS;
END:
    free(edges_to_remove);
    edges_to_remove = NULL;
    return exit_code;
}

int graph_add_edge(graph_t * graph,
                   void *    data_1,
                   void *    data_2,
                   size_t    weight,
                   bool      is_bidirectional)
{
    int      exit_code = E_FAILURE;
    node_t * node_1    = NULL;
    node_t * node_2    = NULL;
    edge_t * edge      = NULL;

    if ((NULL == graph) || (NULL == data_1) || (NULL == data_2))
    {
        print_error("graph_add_edge(): NULL argument passed.");
        goto END;
    }

    node_1 = graph_find_node(graph, data_1);
    if (NULL == node_1)
    {
        print_error("graph_add_edge(): Node 1 does not exist.");
        goto END;
    }

    node_2 = graph_find_node(graph, data_2);
    if (NULL == node_2)
    {
        print_error("graph_add_edge(): Node 2 does not exist.");
        goto END;
    }

    edge = calloc(1, sizeof(edge_t));
    if (NULL == edge)
    {
        print_error("graph_add_edge(): CMR failure.");
        goto END;
    }

    edge->node_1      = node_1;
    edge->node_2      = node_2;
    edge->weight      = weight;
    edge->is_directed = !is_bidirectional;

    exit_code = list_push_head(node_1->edge_list, edge);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "graph_add_edge(): Unable to add edge to node 1's edge list.");
        free(edge);
        goto END;
    }
    node_1->edge_count += 1;

    if (true == is_bidirectional)
    {
        exit_code = list_push_head(node_2->edge_list, edge);
        if (E_SUCCESS != exit_code)
        {
            print_error(
                "graph_add_edge(): Unable to add edge to node 2's edge list.");
            // Rollback
            list_remove_data(node_1->edge_list, edge);
            node_1->edge_count -= 1;
            free(edge);
            goto END;
        }
        node_2->edge_count += 1;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int graph_remove_edge(graph_t * graph, void * data_1, void * data_2)
{
    int      exit_code = E_FAILURE;
    node_t * node_1    = NULL;
    node_t * node_2    = NULL;
    edge_t * edge      = NULL;

    if ((NULL == graph) || (NULL == data_1) || (NULL == data_2))
    {
        print_error("graph_remove_edge(): NULL argument passed.");
        goto END;
    }

    node_1 = graph_find_node(graph, data_1);
    if (NULL == node_1)
    {
        print_error("graph_remove_edge(): Unable to find node 1.");
        goto END;
    }

    node_2 = graph_find_node(graph, data_2);
    if (NULL == node_2)
    {
        print_error("graph_remove_edge(): Unable to find node 2.");
        goto END;
    }

    edge = graph_find_edge(graph, node_1, node_2);
    if (NULL == edge)
    {
        print_error("graph_remove_edge(): Unable to find edge.");
        goto END;
    }

    // Remove the edge from node 1's edge list
    exit_code = list_remove_data(node_1->edge_list, edge);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "graph_remove_edge(): Unable to remove edge from node 1's edge "
            "list.");
        goto END;
    }
    node_1->edge_count -= 1;

    // Remove the edge from node 2's edge list if the edge is bidirectional
    if (!edge->is_directed)
    {
        exit_code = list_remove_data(node_2->edge_list, edge);
        if (E_SUCCESS != exit_code)
        {
            print_error(
                "graph_remove_edge(): Unable to remove edge from node 2's edge "
                "list.");
            // Rollback previous removal from node 1's list
            list_push_head(node_1->edge_list, edge);
            node_1->edge_count += 1;
            goto END;
        }
        node_2->edge_count -= 1;
    }

    // Free the edge memory
    free(edge);
    edge = NULL;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

size_t graph_get_size(graph_t * graph)
{
    size_t size = 0;

    if (NULL == graph)
    {
        print_error("graph_get_size(): NULL argument passed.");
        goto END;
    }

    size = graph->node_count;

END:
    return size;
}

void graph_print(graph_t * graph, ACTION_F custom_print)
{
    list_node_t * current    = NULL;
    node_t *      graph_node = NULL;

    if ((NULL == graph) || (NULL == custom_print))
    {
        print_error("graph_print(): NULL argument passed.");
        goto END;
    }

    current = graph->node_list->head;
    while (NULL != current)
    {
        graph_node = (node_t *)(current->data);
        printf("Node: ");
        custom_print(graph_node->data);
        printf(" with %zu edges.\n", graph_node->edge_count);

        current = current->next;
    }

END:
    return;
}

int graph_clear(graph_t * graph)
{
    int           exit_code = E_FAILURE;
    list_node_t * current   = NULL;
    list_node_t * next      = NULL;
    node_t *      node      = NULL;

    if (NULL == graph)
    {
        print_error("graph_clear(): NULL argument passed.");
        goto END;
    }

    current = graph->node_list->head;
    while (NULL != current)
    {
        node = (node_t *)current->data;
        next = current->next;

        cleanup_node(graph, node);

        current = next;
    }

    exit_code = list_delete(&(graph->node_list));
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_clear(): Unable to reset graph's node list.");
        goto END;
    }

    graph->node_list  = NULL;
    graph->node_count = 0;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int graph_destroy(graph_t ** graph)
{
    int exit_code = E_FAILURE;

    if ((graph == NULL) || (*graph == NULL))
    {
        print_error("graph_destroy(): NULL graph pointer.");
        goto END;
    }

    exit_code = graph_clear(*graph);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_destroy(): Unable to clear graph.");
        goto END;
    }

    // Finally, free the graph structure
    free(*graph);
    *graph = NULL; // Avoid dangling pointer

END:
    return exit_code;
}

static int initialize_dfs(graph_t *  graph,
                          void *     start_data,
                          list_t **  visited_list,
                          stack_t ** stack,
                          node_t **  start_node)
{
    int exit_code = E_FAILURE;

    if ((NULL == graph) || (NULL == start_data) || (NULL == visited_list) ||
        (NULL == stack) || (NULL == start_node))
    {
        print_error("initialize_dfs(): NULL argument passed.");
        goto END;
    }

    *start_node = graph_find_node(graph, start_data);
    if (NULL == *start_node)
    {
        print_error("initialize_dfs(): Unable to find start node.");
        goto END;
    }

    *visited_list = list_new(noop_free, node_ptr_comp);
    if (NULL == *visited_list)
    {
        print_error("initialize_dfs(): Failed to create visited list.");
        goto END;
    }

    *stack = stack_init(MAX_STACK_SIZE, free);
    if (NULL == *stack)
    {
        print_error("initialize_dfs(): Failed to create stack.");
        list_delete(visited_list);
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int dfs_visit_node(stack_t * stack, list_t * visited_list, node_t * node)
{
    int exit_code = E_FAILURE;

    if ((NULL == stack) || (NULL == visited_list) || (NULL == node))
    {
        print_error("dfs_visit_node(): NULL argument passed.");
        goto END;
    }

    exit_code = stack_push(stack, node);
    if (E_SUCCESS != exit_code)
    {
        print_error("dfs_visit_node(): Unable to push node onto stack.");
        goto END;
    }

    exit_code = list_push_tail(visited_list, node);
    if (E_SUCCESS != exit_code)
    {
        print_error("dfs_visit_node(): Unable to push node onto visited list.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int traverse_dfs(stack_t * stack, list_t * visited_list, ACTION_F action)
{
    int           exit_code = E_FAILURE;
    node_t *      current   = NULL;
    list_node_t * edge_node = NULL;
    edge_t *      edge      = NULL;
    node_t *      neighbor  = NULL;

    if ((NULL == stack) || (NULL == visited_list) || (NULL == action))
    {
        print_error("traverse_dfs(): NULL argument passed.");
        goto END;
    }

    while (E_FAILURE == stack_is_empty(stack))
    {
        current = (node_t *)stack_pop(stack);
        if (NULL == current)
        {
            print_error("traverse_dfs(): NULL data popped from stack.");
            goto END;
        }

        action(current->data);

        edge_node = current->edge_list->head;
        while (NULL != edge_node)
        {
            edge     = (edge_t *)edge_node->data;
            neighbor = edge->node_2;

            if (false == list_contains(visited_list, neighbor))
            {
                if (E_SUCCESS != dfs_visit_node(stack, visited_list, neighbor))
                {
                    print_error("traverse_dfs(): Unable to visit neighbor.");
                    goto END;
                }
            }

            edge_node = edge_node->next;
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int graph_dfs(graph_t * graph, void * start_data, ACTION_F action)
{
    int       exit_code    = E_FAILURE;
    list_t *  visited_list = NULL;
    stack_t * stack        = NULL;
    node_t *  start_node   = NULL;

    if ((NULL == graph) || (NULL == start_data) || (NULL == action))
    {
        print_error("graph_dfs(): NULL argument passed.");
        goto END;
    }

    exit_code =
        initialize_dfs(graph, start_data, &visited_list, &stack, &start_node);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_dfs(): Unable to initialize DFS.");
        goto END;
    }

    exit_code = dfs_visit_node(stack, visited_list, start_node);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_dfs(): Unable to visit node.");
        goto END;
    }

    exit_code = traverse_dfs(stack, visited_list, action);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_dfs(): Unable to perform DFS traversal.");
        goto END;
    }

END:
    stack_destroy(&stack);
    list_delete(&visited_list);
    return exit_code;
}

static int bfs_visit_node(queue_t * queue, list_t * visited_list, node_t * node)
{
    int exit_code = E_FAILURE;

    if ((NULL == queue) || (NULL == visited_list) || (NULL == node))
    {
        print_error("bfs_visit_node(): NULL argument passed.");
        goto END;
    }

    exit_code = queue_enqueue(queue, node);
    if (E_SUCCESS != exit_code)
    {
        print_error("bfs_visit_node(): Unable to enqueue node.");
        goto END;
    }

    exit_code = list_push_tail(visited_list, node);
    if (E_SUCCESS != exit_code)
    {
        print_error("bfs_visit_node(): Unable to push node onto visited list.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int initialize_bfs(graph_t *  graph,
                          void *     start_data,
                          list_t **  visited_list,
                          queue_t ** queue,
                          node_t **  start_node)
{
    int exit_code = E_FAILURE;

    if ((NULL == graph) || (NULL == start_data) || (NULL == visited_list) ||
        (NULL == queue) || (NULL == start_node))
    {
        print_error("initialize_bfs(): NULL argument passed.");
        goto END;
    }

    *visited_list = list_new(noop_free, node_ptr_comp);
    if (NULL == *visited_list)
    {
        print_error("initialize_dfs(): Failed to create visited list.");
        goto END;
    }

    *start_node = graph_find_node(graph, start_data);
    if (NULL == *start_node)
    {
        print_error("initialize_bfs(): Unable to find start node.");
        goto END;
    }

    *queue = queue_init(free);
    if (NULL == *queue)
    {
        print_error("initialize_bfs(): Failed to create queue.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int traverse_bfs(queue_t * queue, list_t * visited_list, ACTION_F action)
{
    int           exit_code = E_FAILURE;
    node_t *      current   = NULL;
    list_node_t * edge_node = NULL;
    edge_t *      edge      = NULL;
    node_t *      neighbor  = NULL;

    if ((NULL == queue) || (NULL == visited_list) || (NULL == action))
    {
        print_error("traverse_bfs(): NULL argument passed.");
        goto END;
    }

    while (false == queue_is_empty(queue))
    {
        current = (node_t *)queue_dequeue(queue);
        if (NULL == current)
        {
            print_error("traverse_bfs(): NULL data dequeued.");
            goto END;
        }

        action(current->data);

        edge_node = current->edge_list->head;
        while (NULL != edge_node)
        {
            edge     = (edge_t *)edge_node->data;
            neighbor = edge->node_2;

            if (false == list_contains(visited_list, neighbor))
            {
                if (E_SUCCESS != bfs_visit_node(queue, visited_list, neighbor))
                {
                    print_error("traverse_bfs(): Unable to visit neighbor.");
                    goto END;
                }
            }

            edge_node = edge_node->next;
        }
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

int graph_bfs(graph_t * graph, void * start_data, ACTION_F action)
{
    int       exit_code    = E_FAILURE;
    list_t *  visited_list = NULL;
    queue_t * queue        = NULL;
    node_t *  start_node   = NULL;

    if ((NULL == graph) || (NULL == start_data) || (NULL == action))
    {
        print_error("graph_bfs(): NULL argument passed.");
        goto END;
    }

    exit_code =
        initialize_bfs(graph, start_data, &visited_list, &queue, &start_node);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_bfs(): Unable to initialize BFS.");
        goto END;
    }

    exit_code = bfs_visit_node(queue, visited_list, start_node);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_bfs(): Unable to visit node.");
        goto END;
    }

    exit_code = traverse_bfs(queue, visited_list, action);
    if (E_SUCCESS != exit_code)
    {
        print_error("graph_bfs(): Unable to perform BFS traversal.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    queue_destroy(&queue);
    list_delete(&visited_list);
    return exit_code;
}

int graph_dijkstra(graph_t * graph,
                   void *    start_data,
                   void *    end_data,
                   list_t ** path)
{
    (void)graph;
    (void)start_data;
    (void)end_data;
    (void)path;
    return -1;
}

int graph_a_star(graph_t * graph,
                 void *    start_data,
                 void *    end_data,
                 list_t ** path)
{
    (void)graph;
    (void)start_data;
    (void)end_data;
    (void)path;
    return -1;
}

int graph_floyd_warshall(graph_t * graph, size_t *** distances)
{
    (void)graph;
    (void)distances;
    return -1;
}

int graph_is_connected(graph_t * graph, bool * is_connected)
{
    int exit_code = E_FAILURE;

    (void)graph;
    (void)is_connected;

    return exit_code;
}

int graph_is_cyclic(graph_t * graph, bool * is_cyclic)
{
    int exit_code = E_FAILURE;

    (void)graph;
    (void)is_cyclic;

    return exit_code;
}

list_t * graph_find_connected_components(graph_t * graph)
{
    list_t * connected_components = NULL;

    (void)graph;

    return connected_components;
}

int graph_node_degree(graph_t * graph, void * data, size_t * degree)
{
    int exit_code = E_FAILURE;

    (void)graph;
    (void)data;
    (void)degree;

    return exit_code;
}

list_t * graph_get_adjacent_nodes(graph_t * graph, void * data)
{
    (void)graph;
    (void)data;
    return NULL;
}

bool graph_edge_exists(graph_t * graph, void * data_1, void * data_2)
{
    (void)graph;
    (void)data_1;
    (void)data_2;
    return false;
}

size_t graph_get_edge_weight(graph_t * graph, void * data_1, void * data_2)
{
    (void)graph;
    (void)data_1;
    (void)data_2;
    return -1;
}

graph_t * graph_clone(graph_t * graph)
{
    (void)graph;
    return NULL;
}

// STATIC FUNCTIONS

static node_t * graph_create_node(void * data)
{
    node_t * new_node = NULL;

    if (NULL == data)
    {
        print_error("graph_create_node(): NULL data passed.");
        goto END;
    }

    new_node = calloc(1, sizeof(node_t));
    if (NULL == new_node)
    {
        print_error("graph_create_node(): CMR failure.");
        goto END;
    }

    new_node->data       = data;
    new_node->edge_count = 0;
    new_node->edge_list  = list_new(edge_list_free, edge_compare);
    if (NULL == new_node->edge_list)
    {
        print_error("graph_create_node(): CMR failure.");
        free(new_node);
        new_node = NULL;
        goto END;
    }

END:
    return new_node;
}

static node_t * graph_find_node(graph_t * graph, void * data)
{
    list_node_t * list_node = NULL;
    node_t *      node      = NULL;

    if ((NULL == graph) || (NULL == data))
    {
        print_error("graph_find_node(): NULL argument passed.");
        goto END;
    }

    list_node = list_find_first_occurrence(graph->node_list, &data);
    if (NULL == list_node)
    {
        print_error("graph_find_node(): Unable to find first occurrence.");
        goto END;
    }

    node = ((node_t *)list_node->data);

END:
    return node;
}

static edge_t * graph_find_edge(graph_t * graph,
                                node_t *  node_1,
                                node_t *  node_2)
{
    edge_t *      edge      = NULL;
    list_t *      edge_list = NULL;
    list_node_t * current   = NULL;

    if ((NULL == graph) || (NULL == node_1) || (NULL == node_2))
    {
        print_error("graph_find_edge(): NULL argument passed.");
        goto END;
    }

    edge_list = node_1->edge_list;
    if (NULL == edge_list)
    {
        print_error("graph_find_edge(): Node 1 does not have an edge list.");
        goto END;
    }

    // Find the edge that connects to node 2
    current = edge_list->head;
    while (NULL != current)
    {
        edge = (edge_t *)current->data;

        // Check if the current edge connects to node 2
        if (((edge->node_1 == node_1) && (edge->node_2 == node_2)) ||
            ((edge->node_1 == node_2) && (edge->node_2 == node_1) &&
             false == edge->is_directed))
        {
            // Found the edge
            goto END;
        }

        current = current->next;
    }

END:
    return edge;
}

static edge_t ** collect_edges(node_t * node, uint32_t * num_edges)
{
    edge_t **     edge_list = NULL;
    list_node_t * current   = NULL;
    size_t        index     = 0;

    if ((NULL == node) || (NULL == num_edges))
    {
        print_error("collect_edges(): NULL argument passed.");
        goto END;
    }

    if (NULL == node->edge_list)
    {
        print_error("collect_edges(): NULL edge list.");
        goto END;
    }

    *num_edges = node->edge_list->size;

    edge_list = calloc(*num_edges, sizeof(edge_t *));
    if (NULL == edge_list)
    {
        print_error("collect_edges(): CMR failure.");
        goto END;
    }

    current = node->edge_list->head;
    while ((NULL != current) && (index < *num_edges))
    {
        edge_list[index++] = (edge_t *)current->data;
        current            = current->next;
    }

END:
    return edge_list;
}

static int remove_edges(graph_t * graph,
                        edge_t ** edges_to_remove,
                        size_t    num_edges)
{
    int exit_code = E_FAILURE;

    if ((NULL == graph) || (NULL == edges_to_remove))
    {
        print_error("remove_edges(): NULL argument passed.");
        goto END;
    }

    for (size_t idx = 0; idx < num_edges; ++idx)
    {
        exit_code = graph_remove_edge(graph,
                                      edges_to_remove[idx]->node_1->data,
                                      edges_to_remove[idx]->node_2->data);
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

static void cleanup_node(graph_t * graph, node_t * node)
{
    int exit_code = E_FAILURE;

    if ((NULL == graph) || (NULL == node))
    {
        print_error("cleanup_node(): NULL argument passed.");
        goto END;
    }

    // Free the nodes data and the node itself, and remove the edge list
    graph->custom_free(node->data);
    node->data = NULL;

    node->edge_count = 0;

    list_delete(&(node->edge_list));
    free(node);
    node = NULL;

    (void)exit_code;
END:
    return;
}

/*** end of file ***/
