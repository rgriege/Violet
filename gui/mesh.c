#include <GL/glew.h>

#include "violet/gui/mesh.h"

void vlt_mesh_init(vlt_mesh *m, const v2f *poly, u32 n)
{
	glGenBuffers(1, &m->vbo);
	vlt_mesh_set_vertices(m, poly, n);
}

void vlt_mesh_destroy(vlt_mesh *m)
{
	if (m->vbo != 0)
		glDeleteBuffers(1, &m->vbo);
}

void vlt_mesh_poly(const vlt_mesh *m, array *poly)
{
	array_clear(poly);
	array_reserve(poly, m->sz);
	poly->size = m->sz;
	vlt_mesh_bind(m);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, m->sz * 2 * sizeof(GL_FLOAT),
		poly->data);
}

void vlt_mesh_bind(const vlt_mesh *m)
{
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
}

void vlt_mesh_unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vlt_mesh_set_vertices(vlt_mesh *m, const v2f *v, u32 n)
{
	vlt_mesh_bind(m);
	m->sz = n;
	glBufferData(GL_ARRAY_BUFFER, m->sz * 2 * sizeof(GL_FLOAT), v,
		GL_STATIC_DRAW);
}

