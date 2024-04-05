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
 *
 * @param data_p A pointer to the data to be freed.
 */
typedef void (*FREE_F)(void *);

/**
 * @brief A function pointer type for custom print operations on graph data.
 *
 * @param data_p A pointer to the data to print.
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
    void *   data_p;      /**< Pointer to the data for this node. */
    size_t   edge_count;  /**< The number of edges connected to this node. */
    list_t * edge_list_p; /**< Pointer to a list of edges for this node. */
} node_t;

/**
 * @brief Represents an edge in the graph.
 *
 * An edge connects two nodes, has an associated weight, and can be directed or
 * undirected.
 */
typedef struct
{
    node_t * node_1_p;    /**< Pointer to the first node in the edge. */
    node_t * node_2_p;    /**< Pointer to the second node in the edge. */
    size_t   weight;      /**< Weight of the edge. */
    bool     is_directed; /**< Flag indicating if the edge is directed. */
} edge_t;

/**
 * @brief Represents a graph structure.
 *
 * A graph contains a count of its nodes, a pointer to a list of its nodes, and
 * function pointers for custom free and compare operations.
 */
typedef struct
{
    size_t   node_count;  /**< The number of nodes in the graph. */
    list_t * node_list_p; /**< Pointer to a list containing all nodes in the
                             graph. */
    FREE_F custom_free;   /**< Function pointer for custom free operation on
                             graph's nodes. */
    CMP_F custom_compare; /**< Function pointer for custom comparison operation
                             on graph's nodes. */
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
 * @brief Creates a new node with the given data.
 *
 * @param data_p A pointer to the data to be stored in the node.
 * @return node_t* A pointer to the newly created node or NULL on failure.
 */
node_t * graph_create_node(void * data_p);

/**
 * @brief Adds a new node with the given data to the graph.
 *
 * @param graph_p A pointer to the graph.
 * @param data_p A pointer to the data to be added.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_add_node(graph_t * graph_p, void * data_p);

/**
 * @brief Removes a node with the given data from the graph.
 *
 * @param graph_p A pointer to the graph.
 * @param data_p A pointer to the data identifying the node to be removed.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_remove_node(graph_t * graph_p, void * data_p);

/**
 * @brief Adds an edge between two nodes in the graph.
 *
 * @param graph_p A pointer to the graph.
 * @param data_1_p A pointer to the data of the first node.
 * @param data_2_p A pointer to the data of the second node.
 * @param weight The weight of the edge.
 * @param is_bidirectional A boolean indicating if the edge is bidirectional.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_add_edge(graph_t * graph_p,
                   void *    data_1_p,
                   void *    data_2_p,
                   size_t    weight,
                   bool      is_bidirectional);

/**
 * @brief Removes an edge between two nodes in the graph.
 *
 * @param graph_p A pointer to the graph.
 * @param data_1_p A pointer to the data of the first node.
 * @param data_2_p A pointer to the data of the second node.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_remove_edge(graph_t * graph_p, void * data_1_p, void * data_2_p);

/**
 * @brief Returns the number of nodes in the graph.
 *
 * @param graph_p A pointer to the graph.
 * @return size_t The number of nodes in the graph.
 */
size_t graph_get_size(graph_t * graph_p);

/**
 * @brief Finds a node in the graph with the given data.
 *
 * @param graph_p A pointer to the graph.
 * @param data_p A pointer to the data of the node to find.
 * @return node_t* A pointer to the found node or NULL if not found.
 */
node_t * graph_find_node(graph_t * graph_p, void * data_p);

/**
 * @brief Finds an edge between two nodes in the graph.
 *
 * @param graph_p A pointer to the graph.
 * @param node_1_p A pointer to the first node.
 * @param node_2_p A pointer to the second node.
 * @return edge_t* A pointer to the found edge or NULL if not found.
 */
edge_t * graph_find_edge(graph_t * graph_p,
                         node_t *  node_1_p,
                         node_t *  node_2_p);

void graph_print(graph_t * graph_p, PRINT_F custom_print);

/**
 * @brief Clears the graph, removing all nodes and edges.
 *
 * @param graph_p A pointer to the graph to clear.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_clear(graph_t * graph_p);

/**
 * @brief Destroys the graph, freeing all resources.
 *
 * @param graph_pp A pointer to a pointer to the graph to be destroyed.
 * @return int An integer indicating success (E_SUCCESS) or failure (E_FAILURE).
 */
int graph_destroy(graph_t ** graph_pp);
#endif /* _ADJACENCY_LIST_H */

/*** end of file ***/
