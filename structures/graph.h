#ifndef VIOLET_GRAPH_H
#define VIOLET_GRAPH_H

#include "violet/core/types.h"

typedef struct graph
{
	array nodes;
	array edge_pointers;
	array edges;
} graph;

typedef struct graph_edge
{
	u32 source;
	u32 dest;
	void * edge_data;
} graph_edge;

void graph_init(graph * g);

u32 graph_add_node(graph * g, void * data);
b8 graph_has_node(const graph * g, u32 node);
void * graph_get_node_data(graph * g, u32 node);
u32 graph_node_count(const graph * g);
void * graph_begin_nodes(graph * g);
void * graph_end_nodes(graph * g);

b8 graph_has_edge(graph * g, u32 source, u32 dest);
void graph_add_edge(graph * g, u32 source, u32 dest, void * edge_data);
/*
* Used to add multiple edges at once.
* Adding many edges using AddEdge(...) is highly inefficient,
* since potentially lots of edge pointers will need updating.
*/
void graph_add_edges(graph * g);
void graph_get_edges(const graph * g, u32 src);
void graph_get_edges(const graph * g, u32 src);

void graph_clear(graph * g);
void graph_destroy(graph * g);

#endif
