#ifndef VIOLET_MESH_H
#define VIOLET_MESH_H

#include "violet/core/types.h"
#include "violet/math/r32.h"
#include "violet/structures/array.h"

typedef struct vlt_mesh
{
	u32 vbo;
	u32 sz;
} vlt_mesh;

void vlt_mesh_init(vlt_mesh *m, const v2f *poly, u32 n);
void vlt_mesh_destroy(vlt_mesh *m);

void vlt_mesh_poly(const vlt_mesh *m, array *poly);

void vlt_mesh_bind(const vlt_mesh *m);
void vlt_mesh_unbind();

void vlt_mesh_set_vertices(vlt_mesh *m, const v2f *v, u32 n);

#endif
