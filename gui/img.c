#include <assert.h>
#include <GL/glew.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "violet/math/aabb.h"
#include "violet/math/math.h"
#include "violet/math/poly.h"
#include "violet/utility/log.h"

#include "violet/gui/img.h"
#include "violet/gui/mesh.h"
#include "violet/gui/shader.h"
#include "violet/gui/texture.h"


typedef struct vlt_img
{
	u32 vao;
	vlt_texture texture;
	vlt_mesh mesh;
	vlt_mesh tex_coords;
} vlt_img;

vlt_img * vlt_img_create()
{
	return calloc(sizeof(vlt_img), 1);
}

void vlt_img_free(vlt_img * img)
{
	free(img);
}

b8 vlt_img_load(vlt_img * img, const char * filename)
{
	b8 retval = false;

	glGenVertexArrays(1, &img->vao);
	glBindVertexArray(img->vao);

	if (!vlt_load_png(&img->texture, filename))
	{
		log_write("img_load error");
		return retval;
	}

	aabb box;
	aabb_from_dims(&box, 0, img->texture.height, img->texture.width, 0);
	poly p;
	poly_from_box(&p, &box);

	vlt_mesh_init(&img->mesh, &p.vertices);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	vlt_texture_coords_from_poly(&img->tex_coords, &p);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	array_destroy(&p.vertices);
	glBindVertexArray(0);
	retval = true;
	return retval;
}

void vlt_img_destroy(vlt_img * img)
{
	vlt_mesh_destroy(&img->mesh);
	vlt_mesh_destroy(&img->tex_coords);
	vlt_texture_destroy(&img->texture);
	glDeleteVertexArrays(1, &img->vao);
}

void vlt_img_render(vlt_img * img, s32 x, s32 y, vlt_shader_program * p)
{
	glBindVertexArray(img->vao);

	vlt_texture_bind(&img->texture);

	const GLint offset_attrib = vlt_shader_program_uniform(p,
		"offset");
	glUniform2f(offset_attrib, x, y);

	glDrawArrays(GL_TRIANGLE_FAN, 0, img->mesh.sz);

	vlt_texture_unbind();

	glBindVertexArray(0);
}

