#ifndef _ADJACENCY_LIST_H
#define _ADJACENCY_LIST_H

#include <stdbool.h>
#include <stdint.h>

#include "callback_types.h"
#include "linked_list.h"

/**
 * @brief Structure representing a node in the graph.
 */
typedef struct
{
    void *   data;       // Pointer to the data stored in the node
    size_t   edge_count; // Number of edges connected to the node
    list_t * edge_list;  // List of edges connected to the node
} node_t;

/**
 * @brief Structure representing an edge in the graph.
 */
typedef struct
{
    node_t * node_1;      // Pointer to the first node of the edge
    node_t * node_2;      // Pointer to the second node of the edge
    size_t   weight;      // Weight of the edge
    bool     is_directed; // Flag indicating if the edge is directed
} edge_t;

/**
 * @brief Structure representing a graph.
 */
typedef struct
{
    size_t   node_count;     // Number of nodes in the graph
    list_t * node_list;      // List of nodes in the graph
    FREE_F   custom_free;    // Custom free function
    CMP_F    custom_compare; // Custom compare function
} graph_t;

/**
 * @brief Creates a new graph.
 *
 * @param custom_free Function pointer to the custom free function.
 * @param custom_compare Function pointer to the custom compare function.
 * @return Pointer to the newly created graph, or NULL on failure.
 */
graph_t * graph_create(FREE_F custom_free, CMP_F custom_compare);

/**
 * @brief Adds a node to the graph.
 *
 * @param graph Pointer to the graph.
 * @param data Pointer to the data to be added.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_add_node(graph_t * graph, void * data);

/**
 * @brief Removes a node from the graph.
 *
 * @param graph Pointer to the graph.
 * @param data Pointer to the data of the node to be removed.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_remove_node(graph_t * graph, void * data);

/**
 * @brief Adds an edge between two nodes in the graph.
 *
 * @param graph Pointer to the graph.
 * @param data_1 Pointer to the data of the first node.
 * @param data_2 Pointer to the data of the second node.
 * @param weight Weight of the edge.
 * @param is_bidirectional Flag indicating if the edge is bidirectional.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_add_edge(graph_t * graph,
                   void *    data_1,
                   void *    data_2,
                   size_t    weight,
                   bool      is_bidirectional);

/**
 * @brief Removes an edge between two nodes in the graph.
 *
 * @param graph Pointer to the graph.
 * @param data_1 Pointer to the data of the first node.
 * @param data_2 Pointer to the data of the second node.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_remove_edge(graph_t * graph, void * data_1, void * data_2);

/**
 * @brief Gets the size of the graph.
 *
 * @param graph Pointer to the graph.
 * @return Size of the graph (number of nodes).
 */
size_t graph_get_size(graph_t * graph);

/**
 * @brief Prints the graph using a custom print function.
 *
 * @param graph Pointer to the graph.
 * @param custom_print Function pointer to the custom print function.
 */
void graph_print(graph_t * graph, ACTION_F custom_print);

/**
 * @brief Clears all nodes and edges from the graph.
 *
 * @param graph Pointer to the graph.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_clear(graph_t * graph);

/**
 * @brief Destroys the graph and frees all associated memory.
 *
 * @param graph Pointer to the pointer to the graph.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_destroy(graph_t ** graph);

/**
 * @brief Performs depth-first search (DFS) on the graph.
 *
 * @param graph Pointer to the graph.
 * @param start_data Pointer to the data of the starting node.
 * @param action Function pointer to the custom action function.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_dfs(graph_t * graph, void * start_data, ACTION_F action);

/**
 * @brief Performs breadth-first search (BFS) on the graph.
 *
 * @param graph Pointer to the graph.
 * @param start_data Pointer to the data of the starting node.
 * @param action Function pointer to the custom action function.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_bfs(graph_t * graph, void * start_data, ACTION_F action);

/**
 * @brief Finds the shortest path between two nodes using Dijkstra's algorithm.
 *
 * @param graph Pointer to the graph.
 * @param start_data Pointer to the data of the starting node.
 * @param end_data Pointer to the data of the ending node.
 * @param path Pointer to the list to store the path.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_dijkstra(graph_t * graph,
                   void *    start_data,
                   void *    end_data,
                   list_t ** path);

/**
 * @brief Finds the shortest path between two nodes using A* algorithm.
 *
 * @param graph Pointer to the graph.
 * @param start_data Pointer to the data of the starting node.
 * @param end_data Pointer to the data of the ending node.
 * @param path Pointer to the list to store the path.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_a_star(graph_t * graph,
                 void *    start_data,
                 void *    end_data,
                 list_t ** path);

/**
 * @brief Computes the shortest paths between all pairs of nodes using
 * Floyd-Warshall algorithm.
 *
 * @param graph Pointer to the graph.
 * @param distances Pointer to the 2D array to store the distances.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_floyd_warshall(graph_t * graph, size_t *** distances);

/**
 * @brief Checks if the graph is connected.
 *
 * @param graph Pointer to the graph.
 * @param is_connected Pointer to the boolean to store the result.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_is_connected(graph_t * graph, bool * is_connected);

/**
 * @brief Checks if the graph contains a cycle.
 *
 * @param graph Pointer to the graph.
 * @param is_cyclic Pointer to the boolean to store the result.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_is_cyclic(graph_t * graph, bool * is_cyclic);

/**
 * @brief Finds the connected components in the graph.
 *
 * @param graph Pointer to the graph.
 * @return Pointer to the list of connected components.
 */
list_t * graph_find_connected_components(graph_t * graph);

/**
 * @brief Gets the degree of a node in the graph.
 *
 * @param graph Pointer to the graph.
 * @param data Pointer to the data of the node.
 * @param degree Pointer to the size_t to store the degree.
 * @return E_SUCCESS on success, or E_FAILURE on failure.
 */
int graph_node_degree(graph_t * graph, void * data, size_t * degree);

/**
 * @brief Gets the adjacent nodes of a given node in the graph.
 *
 * @param graph Pointer to the graph.
 * @param data Pointer to the data of the node.
 * @return Pointer to the list of adjacent nodes.
 */
list_t * graph_get_adjacent_nodes(graph_t * graph, void * data);

/**
 * @brief Checks if an edge exists between two nodes in the graph.
 *
 * @param graph Pointer to the graph.
 * @param data_1 Pointer to the data of the first node.
 * @param data_2 Pointer to the data of the second node.
 * @return true if the edge exists, false otherwise.
 */
bool graph_edge_exists(graph_t * graph, void * data_1, void * data_2);

/**
 * @brief Gets the weight of an edge between two nodes in the graph.
 *
 * @param graph Pointer to the graph.
 * @param data_1 Pointer to the data of the first node.
 * @param data_2 Pointer to the data of the second node.
 * @return Weight of the edge.
 */
size_t graph_get_edge_weight(graph_t * graph, void * data_1, void * data_2);

/**
 * @brief Creates a clone of the graph.
 *
 * @param graph Pointer to the graph.
 * @return Pointer to the cloned graph.
 */
graph_t * graph_clone(graph_t * graph);

#endif /* _ADJACENCY_LIST_H */
