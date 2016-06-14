#ifndef VIOLET_STRAIGHT_SKELETON_H
#define VIOLET_STRAIGHT_SKELETON_H

#include "violet/structures/array.h"
#include "violet/structures/array_map.h"
#include "violet/math/poly.h"

typedef struct ss_edge_pair
{
	u32 dst_idx0;
	u32 dst_idx1;
} ss_edge_pair;
typedef struct straight_skeleton
{
	poly * src;
	array spine_vertices;
	array_map edges;
} straight_skeleton;

#include "violet/structures/pqueue.h"
typedef struct ss_debug
{
	b8 init;
	b8 cc;
	array active_vertex_lists;
	pqueue isecs;
} ss_debug;
typedef struct ss_debug_edge
{
	v2f start;
	v2f end;
} ss_debug_edge;
typedef struct ss_debug_vtx
{
	u32 src_idx;
	ss_debug_edge edges[2];
	v2f bisector;
	b8 reflex;
} ss_debug_vtx;
typedef struct ss_debug_isec
{
	r32 t;
	v2f p;
	u32 src_idcs[2];
	u32 split_src_idx;
	u32 vertex_list_idx;
} ss_debug_isec;

void poly_straight_skeleton(poly * p, straight_skeleton * s);
b8 poly_ss_debug(poly * p, straight_skeleton * s, ss_debug * dbg);
const v2f * ss_vertex(straight_skeleton * s, u32 idx);
void straight_skeleton_destroy(straight_skeleton * s);

void polygon_inset(straight_skeleton * s, r32 inset, poly * out);

#endif
