# Adjacency List README

## Overview

This guide explains how to use the adjacency list data structure for graph representation. The provided implementation allows for the creation of directed and undirected graphs with nodes and edges, where each node can store generic data.

## Compilation

Ensure your C files are correctly referenced in your makefile or compilation command. If you're using GCC, the compilation command might look something like this:

```sh
gcc -o graph_program main.c adjacency_list.c linked_list.c utilities.c -I.
```

Make sure to replace `main.c`, `adjacency_list.c`, `linked_list.c`, and `utilities.c` with the actual paths to your source files. The `-I.` flag tells the compiler to look for header files in the current directory.

## Usage

### Creating a Graph

Before using the graph, include its header file in your source code:

```c
#include "adjacency_list.h"
```

To create a new graph, use the `graph_create` function. You need to provide custom free and compare functions for the data stored in the nodes:

```c
void custom_free(void *data);
int custom_compare(const void *a, const void *b);

graph_t *graph = graph_create(custom_free, custom_compare);
```

### Adding Nodes

To add a node to the graph, use the `graph_add_node` function. The data for the node must be allocated and passed as a pointer:

```c
graph_add_node(graph, "A");
graph_add_node(graph, "B");
graph_add_node(graph, "C");
```

### Adding Edges

To add edges between nodes, use the `graph_add_edge` function. You need to specify the data of the start and end nodes, the weight of the edge, and whether the edge is bidirectional:

```c
graph_add_edge(graph, "A", "B", 1, false); // Adds a directed edge from A to B
graph_add_edge(graph, "A", "C", 1, true);  // Adds an undirected (bidirectional) edge between A and C
```

### Removing Nodes and Edges

To remove a node or an edge, use the `graph_remove_node` or `graph_remove_edge` function respectively:

```c
graph_remove_edge(graph, "A", "B");
graph_remove_node(graph, "C");
```

### Destroying the Graph

To free the memory allocated for the graph and its components, use the `graph_destroy` function:

```c
graph_destroy(&graph);
```

## Example

Here's an example of creating a graph, adding nodes and edges, and then cleaning up:

```c
#include "adjacency_list.h"

int main() {
    // Assume custom_free and custom_compare are implemented elsewhere
    graph_t *graph = graph_create(custom_free, custom_compare);

    // Add nodes
    graph_add_node(graph, "A");
    graph_add_node(graph, "B");
    graph_add_node(graph, "C");
    graph_add_node(graph, "D");

    // Add edges
    graph_add_edge(graph, "A", "B", 1, false);
    graph_add_edge(graph, "B", "C", 1, true);
    graph_add_edge(graph, "C", "D", 1, false);

    // Your graph usage here

    // Clean up
    graph_destroy(&graph);

    return 0;
}
```

Ensure you have defined `custom_free` and `custom_compare` functions that know how to free your data and compare two data nodes, respectively.