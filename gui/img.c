#include <assert.h>
#include <GL/glew.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "violet/math/r32.h"
#include "violet/utility/log.h"

#include "violet/gui/img.h"
#include "violet/gui/mesh.h"
#include "violet/gui/shader.h"
#include "violet/gui/texture.h"


b8 vlt_img_load(vlt_img *img, const char *filename)
{
	b8 retval = false;

	glGenVertexArrays(1, &img->vao);
	glBindVertexArray(img->vao);

	if (!vlt_load_png(&img->texture, filename))
	{
		log_write("img_load(%s) error", filename);
		return retval;
	}

	box2f box;
	box2f_from_dims(&box, 0, img->texture.height, img->texture.width, 0);
	v2f poly[4];
	polyf_from_box(poly, &box);

	vlt_mesh_init(&img->mesh, poly, 4);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	vlt_texture_coords_from_poly(&img->tex_coords, poly, 4);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	retval = true;
	return retval;
}

void vlt_img_destroy(vlt_img *img)
{
	vlt_mesh_destroy(&img->mesh);
	vlt_mesh_destroy(&img->tex_coords);
	vlt_texture_destroy(&img->texture);
	glDeleteVertexArrays(1, &img->vao);
}

void vlt_img_render(const vlt_img *img, s32 x, s32 y, r32 sx, r32 sy,
                    vlt_shader_program *p)
{
	glBindVertexArray(img->vao);

	vlt_texture_bind(&img->texture);

	const GLint offset_attrib = vlt_shader_program_uniform(p, "offset");
	glUniform2f(offset_attrib, x, y);

	const GLint scale_attrib = vlt_shader_program_uniform(p, "scale");
	glUniform2f(scale_attrib, sx, sy);

	glDrawArrays(GL_TRIANGLE_FAN, 0, img->mesh.sz);

	vlt_texture_unbind();

	glBindVertexArray(0);
}

