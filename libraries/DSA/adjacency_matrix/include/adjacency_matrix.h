/**
 * @file adjacency_matrix.h
 *
 * @brief
 */
#ifndef _ADJACENCY_MATRIX_H
#define _ADJACENCY_MATRIX_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "callback_types.h"

typedef void (*FREE_F)(void *);
typedef bool (*PREDICATE_F)(const void *, const char *);
typedef size_t (*HEURISTIC_F)(const char *, const char *);

typedef struct matrix matrix_t;
typedef struct node   node_t;
typedef struct edge   edge_t;

// Primary API functions

/**
 * @brief Initializes a new graph represented by an adjacency matrix.
 *
 * @param num_nodes The initial number of nodes in the graph.
 * @param is_directed Flag indicating whether the graph is directed.
 * @param custom_compare Pointer to a custom comparison function for node data.
 * @param custom_free Pointer to a custom free function for node data.
 * @return A pointer to the newly created graph, or NULL if initialization
 * fails.
 */
matrix_t * matrix_init(size_t num_nodes,
                       bool   is_directed,
                       CMP_F  custom_compare,
                       FREE_F custom_free);

/**
 * @brief Destroys the graph and frees all associated memory.
 *
 * @param graph_address Pointer to the graph to be destroyed.
 */
void matrix_destroy(matrix_t ** graph_address);

/**
 * @brief Populates a pre-allocated node in the graph with data and a label.
 *
 * @param graph Pointer to the graph.
 * @param index The index of the node to populate.
 * @param data Pointer to the data to assign to the node.
 * @param label The label to assign to the node.
 * @return Status code indicating success or failure.
 */
int matrix_populate_node(matrix_t * graph,
                         size_t     index,
                         void *     data,
                         char *     label);

/**
 * @brief Depopulates a node in the graph by freeing the data and the label.
 *
 * @param graph Pointer to the graph.
 * @param index The index of the node to depopulate.
 * @return Status code indicating success or failure.
 */
int matrix_depopulate_node(matrix_t * graph, size_t index);

/**
 * @brief Adds a new node to the graph.
 *
 * @param graph Pointer to the graph.
 * @param data Pointer to the node's data.
 * @param label The label of the node.
 * @return Status code indicating success or failure.
 */
int matrix_add_node(matrix_t * graph, const void * data, char * label);

/**
 * @brief Removes a node and its associated edges from the graph.
 *
 * @param graph Pointer to the graph.
 * @param label The label of the node to remove.
 * @return Status code indicating success or failure.
 */
int matrix_remove_node(matrix_t * graph, char * label);

/**
 * @brief Retrieves the data associated with a specific node.
 *
 * @param graph Pointer to the graph.
 * @param label The label of the node.
 * @param data Out parameter for the node's data.
 * @return Status code indicating success or failure.
 */
int matrix_get_node_data(const matrix_t * graph,
                         const char *     label,
                         void **          data);

/**
 * @brief Adds an edge between two nodes specified by their labels.
 *
 * @param graph Pointer to the graph.
 * @param src_label The label of the source node.
 * @param dst_label The label of the destination node.
 * @param weight The weight of the edge.
 * @return Status code indicating success or failure.
 */
int matrix_add_edge_by_label(matrix_t *   graph,
                             const char * src_label,
                             const char * dst_label,
                             size_t       weight);

/**
 * @brief Removes an edge between two nodes specified by their labels.
 *
 * @param graph Pointer to the graph.
 * @param src_label The label of the source node.
 * @param dst_label The label of the destination node.
 * @return Status code indicating success or failure.
 */
int matrix_remove_edge_by_label(matrix_t *   graph,
                                const char * src_label,
                                const char * dst_label);

/**
 * @brief Sets the weight of an existing edge between two nodes.
 *
 * @param graph Pointer to the graph.
 * @param src_label The label of the source node.
 * @param dst_label The label of the destination node.
 * @param edge_weight The new weight of the edge.
 * @return Status code indicating success or failure.
 */
int matrix_set_edge_weight_by_label(matrix_t *   graph,
                                    const char * src_label,
                                    const char * dst_label,
                                    size_t       edge_weight);

/**
 * @brief Gets the weight of an edge between two nodes.
 *
 * @param graph Pointer to the graph.
 * @param src_label The label of the source node.
 * @param dst_label The label of the destination node.
 * @param edge_weight Out parameter for the weight of the edge.
 * @return Status code indicating success or failure.
 */
int matrix_get_edge_weight_by_label(const matrix_t * graph,
                                    const char *     src_label,
                                    const char *     dst_label,
                                    size_t *         edge_weight);

/**
 * @brief Checks if there is an edge between two nodes.
 *
 * @param graph Pointer to the graph.
 * @param src_label The label of the source node.
 * @param dst_label The label of the destination node.
 * @return 'true' if an edge exists, 'false' otherwise.
 */
int matrix_has_edge_by_label(const matrix_t * graph,
                             const char *     src_label,
                             const char *     dst_label);

/**
 * @brief Retrieves the indices of all neighbors of a node.
 *
 * @param graph Pointer to the graph.
 * @param node_label The label of the node.
 * @param num_neighbors Out parameter for the number of adjacent neighbors.
 * @param neighbors Out parameter for an array of indices of neighboring nodes.
 * @return Status code indicating success or failure.
 */
int matrix_get_neighbors_by_label(const matrix_t * graph,
                                  const char *     node_label,
                                  size_t *         num_neighbors,
                                  size_t **        neighbors);

/**
 * @brief Returns a deep copy of a graph, without affecting the original.
 *
 * @param graph Pointer to the graph.
 * @return A new copy of the graph
 */
matrix_t * matrix_copy(const matrix_t * graph);

/**
 * @brief Performs a Breadth-First Search (BFS) starting from a given node.
 *
 * @param graph The graph on which to perform the BFS.
 * @param start_label The label of the node from which to start the BFS.
 * @param visit_action Function pointer to a callback function that is called
 *                     for each node as it is visited. The callback receives
 *                     the node's data and its label.
 * @return Status code indicating success or failure.
 */
int matrix_bfs(const matrix_t * graph,
               const char *     start_label,
               ACTION_F         visit_action);

/**
 * @brief Performs a Depth-First Search (DFS) starting from a given node.
 *
 * @param graph The graph on which to perform the DFS.
 * @param start_label The label of the node from which to start the DFS.
 * @param visit_action Function pointer to a callback function that is called
 *                     for each node as it is visited. The callback receives
 *                     the node's data and its label.
 * @return Status code indicating success or failure.
 */
int matrix_dfs(const matrix_t * graph,
               const char *     start_label,
               ACTION_F         visit_action);

/**
 * @brief Performs a flood fill starting from a given node, applying an action
 * to all connected nodes that satisfy a given predicate.
 *
 * @param graph The graph on which to perform the flood fill.
 * @param start_label The label of the node from which to start the fill.
 * @param predicate A function that determines if a node should be filled.
 * Returns true if the node meets the condition, false otherwise.
 * @param fill_action A function that performs an action on each node that is
 * filled. This might involve setting a value or marking the node.
 * @return Status code indicating success or failure.
 */
int matrix_flood_fill(matrix_t *   graph,
                      const char * start_label,
                      PREDICATE_F  predicate,
                      ACTION_F     fill_action);

/**
 * @brief Finds the shortest path between two nodes in a graph using Dijkstra's
 * algorithm.
 *
 * @param graph The graph in which to find the shortest path.
 * @param start_label The label of the starting node.
 * @param target_label The label of the target node.
 * @param path An output parameter for storing the path as a dynamically
 * allocated array of node labels. The caller is responsible for freeing each
 * string and the array itself.
 * @param path_length An output parameter for the number of nodes in the path.
 * @param total_weight An output parameter for the total weight of the path.
 * @return Status code indicating success or failure. Success includes
 * populating `path`, `path_length`, and `total_weight` with the shortest path
 * information.
 */
int matrix_dijkstra_shortest_path(const matrix_t * graph,
                                  const char *     start_label,
                                  const char *     target_label,
                                  char ***         path,
                                  int *            path_length,
                                  size_t *         total_weight);

/**
 * @brief Finds the shortest path between two nodes in a graph using the A*
 * algorithm.
 *
 * @param graph The graph in which to find the shortest path.
 * @param start_label The label of the starting node.
 * @param target_label The label of the target node.
 * @param heuristic Function pointer to a heuristic function that estimates the
 * cost from the current node to the target node. This function should take two
 * node labels as parameters and return an estimated cost as a size_t.
 * @param path An output parameter for storing the path as a dynamically
 * allocated array of node labels. The caller is responsible for freeing each
 * string and the array itself.
 * @param path_length An output parameter for the number of nodes in the path.
 * @param total_weight An output parameter for the total weight of the path,
 * including both the actual path weight and the heuristic estimates.
 * @return Status code indicating success or failure. Success includes
 * populating `path`, `path_length`, and `total_weight` with the shortest path
 * information.
 */
int matrix_astar_shortest_path(const matrix_t * graph,
                               const char *     start_label,
                               const char *     target_label,
                               HEURISTIC_F      heuristic,
                               char ***         path,
                               int *            path_length,
                               size_t *         total_weight);

/**
 * @brief Prints the graph's adjacency matrix and node labels to standard
 * output.
 *
 * @param graph Pointer to the graph.
 */
void matrix_print(const matrix_t * graph);

// Secondary API functions

int matrix_add_edge_by_index(matrix_t * graph,
                             size_t     src_index,
                             size_t     dst_index,
                             size_t     weight);

int matrix_remove_edge_by_index(matrix_t * graph,
                                size_t     src_index,
                                size_t     dst_index);

int matrix_set_edge_weight_by_index(matrix_t * graph,
                                    size_t     src_index,
                                    size_t     dst_index,
                                    size_t     edge_weight);

int matrix_get_edge_weight_by_index(matrix_t * graph,
                                    size_t     src_index,
                                    size_t     dst_index,
                                    size_t *   edge_weight);

int matrix_has_edge_by_index(const matrix_t * graph,
                             size_t           src_index,
                             size_t           dst_index);

int matrix_get_neighbors_by_index(const matrix_t * graph,
                                  size_t           node_index,
                                  size_t **        neighbors);
#endif /* _ADJACENCY_MATRIX_H */

/*** end of file ***/
