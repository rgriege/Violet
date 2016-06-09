#include <GL/glew.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "violet/gui/mesh.h"
#include "violet/gui/texture.h"
#include "violet/math/aabb.h"
#include "violet/math/poly.h"
#include "violet/utility/log.h"

b8 vlt_load_png(vlt_texture * tex, const char * filename)
{
	b8 ret = false;
	png_image image = {0};
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, filename))
	{
		image.format = PNG_FORMAT_RGBA;
		png_bytep buf = malloc(PNG_IMAGE_SIZE(image));
		if (buf)
		{
			if (png_image_finish_read(&image, NULL, buf, 0, NULL))
			{
				vlt_texture_init(tex, image.width, image.height, GL_RGBA, buf);
				ret = true;
				free(buf);
			}
			else
			{
				log_write("png '%s' read: %s", filename, image.message);
				png_image_free(&image);
			}
		}
		else
			log_write("png read: out of memory");
	}

	return ret;
}

void vlt_texture_init(vlt_texture * tex, u32 w, u32 h, u32 fmt, void * data)
{
	glGenTextures(1, &tex->handle);
	tex->width = w;
	tex->height = h;
	vlt_texture_bind(tex);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	vlt_texture_unbind();
}

void vlt_texture_destroy(vlt_texture * tex)
{
	if (tex->handle != 0)
		glDeleteTextures(1, &tex->handle);
}

void vlt_texture_coords_from_poly(vlt_mesh * tex_coords, const poly * p)
{
	aabb extent;
	aabb_init_point(&extent, array_get(&p->vertices, 0));
	for (u32 i = 1, end = array_size(&p->vertices); i < end; ++i)
	{
		const v2f * vertex = array_get(&p->vertices, i);
		aabb_extend_point(&extent, vertex);
	}
	v2f minimum, dimension;
	aabb_get_min(&extent, &minimum);
	aabb_get_max(&extent, &dimension);
	v2f_sub(&dimension, &minimum, &dimension);

	array coords;
	array_init(&coords, sizeof(v2f));
	array_reserve(&coords, array_size(&p->vertices));
	for (u32 i = 0, end = array_size(&p->vertices); i < end; ++i)
	{
		const v2f * vertex = array_get(&p->vertices, i);
		v2f tex_coord;
		v2f_sub(vertex, &minimum, &tex_coord);
		v2f_div(&tex_coord, &dimension, &tex_coord);
		// TODO(rgriege): figure out why this is necessary (font too)
		tex_coord.v = 1 - tex_coord.v;
		array_append(&coords, &tex_coord);
	}
	vlt_mesh_init(tex_coords, &coords);
	array_destroy(&coords);
}

void vlt_texture_bind(const vlt_texture * tex)
{
	glBindTexture(GL_TEXTURE_2D, tex->handle);
}

void vlt_texture_unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

