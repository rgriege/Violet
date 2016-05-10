#include <assert.h>

#include "violet/structures/array.h"
#include "violet/structures/graph.h"

/*void graph_init(graph * g)
{
	graph temp = {
		.nodes = array_create(sizeof(void*)),
		.edge_pointers = array_create(sizeof(u32)),
		.edges = array_create(sizeof(graph_edge))
	};
	memcpy(g, &temp, sizeof(*g));
}

u32 graph_add_node(graph * g, void * data)
{
	array_add(g->nodes, data);
	return array_size(g->nodes) - 1;
}

b8 graph_has_node(const graph * g, u32 node)
{
	return array_size(g->nodes) < node;
}

void * graph_get_node_data(graph * g, u32 node)
{
	assert(node < array_size(g->nodes));
	return array_get(g->nodes, node);
}

u32 graph_node_count(const graph * g)
{
	return array_size(g->nodes);
}

void * graph_begin_nodes(graph * g)
{
	return array_get(g->nodes, 0);
}

void * graph_end_nodes(graph * g)
{
	return array_get(g->nodes, array_size(g->nodes));
}

b8 graph_has_edge(graph * g, u32 source, u32 dest);
void graph_add_edge(graph * g, u32 source, u32 dest, void * edge_data);
void graph_add_edges(graph * g);
u32 graph_edge_count(const graph * g);

void graph_clear(graph * g)
{
	array_clear(g->nodes);
	array_clear(g->edge_pointers);
	array_clear(g->edges);
}

void graph_destroy(graph * g)
{
	array_destroy(g->nodes);
	array_destroy(g->edge_pointers);
	array_destroy(g->edges);
	free(g);
}*/
