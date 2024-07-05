/**
 * @file adjacency_list.h
 *
 * @brief
 */
#ifndef _ADJACENCY_LIST_H
#define _ADJACENCY_LIST_H

#include <stdbool.h>
#include <stdint.h>

#include "linked_list.h"

/**
 * @brief A function pointer type for custom free operations on graph data.
 */
typedef void (*FREE_F)(void *);

/**
 * @brief A function pointer type for custom print operations on graph data.
 */
typedef void (*PRINT_F)(const void *);

/**
 * @brief Represents a node in the graph.
 *
 * A node contains user-defined data, a count of its outgoing/incoming edges,
 * and a pointer to a list of its edges.
 */
typedef struct
{
    void *   data;       // Pointer to the data for this node.
    size_t   edge_count; // The number of edges connected to this node.
    list_t * edge_list;  // Pointer to a list of edges for this node.
} node_t;

/**
 * @brief Represents an edge in the graph.
 *
 * An edge connects two nodes, has an associated weight, and can be directed or
 * undirected.
 */
typedef struct
{
    node_t * node_1;      // Pointer to the first node in the edge.
    node_t * node_2;      // Pointer to the second node in the edge.
    size_t   weight;      // Weight of the edge.
    bool     is_directed; // Flag indicating if the edge is directed.
} edge_t;

/**
 * @brief Represents a graph structure.
 *
 * A graph contains a count of its nodes, a pointer to a list of its nodes, and
 * function pointers for custom free and compare operations.
 */
typedef struct
{
    size_t   node_count; // The number of nodes in the graph.
    list_t * node_list;  // Pointer to a list containing all nodes in the graph.
    FREE_F custom_free; // Function pointer for custom free operation on graph's
                        // nodes.
    CMP_F custom_compare; // Function pointer for custom comparison operation on
                          // graph's nodes.
} graph_t;

/**
 * @brief Creates a new graph.
 *
 * @param custom_free A function pointer for custom free operation.
 * @param custom_compare A function pointer for custom comparison operation.
 * @return graph_t* A pointer to the newly created graph or NULL on failure.
 */
graph_t * graph_create(FREE_F custom_free, CMP_F custom_compare);

/**
 * @brief Adds a new node with the given data to the graph.
 *
 * @param graph A pointer to the graph.
 * @param data A pointer to the data to be added.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_add_node(graph_t * graph, void * data);

/**
 * @brief Removes a node with the given data from the graph.
 *
 * @param graph A pointer to the graph.
 * @param data A pointer to the data identifying the node to be removed.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_remove_node(graph_t * graph, void * data);

/**
 * @brief Adds an edge between two nodes in the graph.
 *
 * @param graph A pointer to the graph.
 * @param data_1 A pointer to the data of the first node.
 * @param data_2 A pointer to the data of the second node.
 * @param weight The weight of the edge.
 * @param is_bidirectional A boolean indicating if the edge is bidirectional.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_add_edge(graph_t * graph,
                   void *    data_1,
                   void *    data_2,
                   size_t    weight,
                   bool      is_bidirectional);

/**
 * @brief Removes an edge between two nodes in the graph.
 *
 * @param graph A pointer to the graph.
 * @param data_1 A pointer to the data of the first node.
 * @param data_2 A pointer to the data of the second node.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_remove_edge(graph_t * graph, void * data_1, void * data_2);

/**
 * @brief Returns the number of nodes in the graph.
 *
 * @param graph A pointer to the graph.
 * @return size_t The number of nodes in the graph.
 */
size_t graph_get_size(graph_t * graph);

void graph_print(graph_t * graph, PRINT_F custom_print);

/**
 * @brief Clears the graph, removing all nodes and edges.
 *
 * @param graph A pointer to the graph to clear.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_clear(graph_t * graph);

/**
 * @brief Destroys the graph, freeing all resources.
 *
 * @param graph A pointer to a pointer to the graph to be destroyed.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_destroy(graph_t ** graph);

/**
 * @brief Checks if the graph is connected.
 *
 * This function determines if the graph is connected, meaning there is a path
 * between every pair of nodes.
 *
 * @param graph A pointer to the graph.
 * @param is_connected_p A pointer to a boolean where the result will be stored.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_is_connected(graph_t * graph, bool * is_connected);

/**
 * @brief Checks if the graph contains a cycle.
 *
 * This function determines if the graph contains any cycles.
 *
 * @param graph A pointer to the graph.
 * @param is_cyclic_p A pointer to a boolean where the result will be stored.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_is_cyclic(graph_t * graph, bool * is_cyclic);

/**
 * @brief Finds all connected components in the graph.
 *
 * This function finds all connected components in the graph and returns a list
 * of lists, where each inner list contains the nodes of one connected
 * component.
 *
 * @param graph A pointer to the graph.
 * @return list_t* A pointer to a list containing all connected components,
 * where each connected component is represented as a list of nodes.
 */
list_t * graph_find_connected_components(graph_t * graph);

/**
 * @brief Gets the degree of a node.
 *
 * This function returns the degree of a specified node in the graph.
 *
 * @param graph A pointer to the graph.
 * @param data A pointer to the data identifying the node.
 * @param degree_p A pointer to a size_t where the degree will be stored.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_node_degree(graph_t * graph, void * data, size_t * degree);

#endif /* _ADJACENCY_LIST_H */

/*** end of file ***/
