#include <GL/glew.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "violet/gui/mesh.h"
#include "violet/gui/texture.h"
#include "violet/math/r32.h"
#include "violet/utility/log.h"

b8 vlt_load_png(vlt_texture *tex, const char *filename)
{
	b8 ret = false;
#if PNG_LIBPNG_VER_MINOR == 6 // 1.6
	png_image image = { 0 };
	image.version = PNG_IMAGE_VERSION;
	if (png_image_begin_read_from_file(&image, filename)) {
		image.format = PNG_FORMAT_RGBA;
		png_bytep buf = malloc(PNG_IMAGE_SIZE(image));
		if (buf) {
			if (png_image_finish_read(&image, NULL, buf, 0, NULL)) {
				vlt_texture_init(tex, image.width, image.height, GL_RGBA, buf);
				ret = true;
				free(buf);
			}
			else {
				log_write("png '%s' read: %s", filename, image.message);
				png_image_free(&image);
			}
		}
		else {
			log_write("png read: out of memory");
		}
	}
#else // fallback to 1.2
	FILE *fp;
	png_byte header[8];
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	int bit_depth, color_type;
	GLint format;
	int bytes_per_row;
	png_bytep image_data, *row_pointers;

	if (!(fp = fopen(filename, "rb")))
		goto out;

	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
		goto err_png;

	if (!(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
		goto err_png;

	if (!(info_ptr = png_create_info_struct(png_ptr)))
		goto err;

	if (setjmp(png_jmpbuf(png_ptr)))
		goto err; // no destroy info_ptr?

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	             NULL, NULL, NULL);

	if (bit_depth != 8)
		goto err;

	switch (color_type) {
		case PNG_COLOR_TYPE_RGB:
			format = GL_RGB;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			format = GL_RGBA;
			break;
		default:
			goto err;
	}

	png_read_update_info(png_ptr, info_ptr);

	bytes_per_row = png_get_rowbytes(png_ptr, info_ptr);
	bytes_per_row += 3 - ((bytes_per_row - 1) % 4); // 4-byte aligned

	image_data = malloc(bytes_per_row * height * sizeof(png_byte) + 15);
	row_pointers = malloc(height * sizeof(png_bytep));
	for (u32 i = 0; i < height; ++i)
		row_pointers[i] = image_data + i * bytes_per_row;

	png_read_image(png_ptr, row_pointers);

	vlt_texture_init(tex, width, height, format, image_data);
	ret = true;

	free(row_pointers);
	free(image_data);

err:
	png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
err_png:
	fclose(fp);
out:
#endif
	return ret;
}

void vlt_texture_init(vlt_texture *tex, u32 w, u32 h, u32 fmt, void *data)
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

void vlt_texture_destroy(vlt_texture *tex)
{
	if (tex->handle != 0)
		glDeleteTextures(1, &tex->handle);
}

void vlt_texture_coords_from_poly(vlt_mesh *tex_coords,
                                  const v2f *v, u32 n)
{
	box2f extent;
	polyf_bounding_box(v, n, &extent);
	v2f dimension;
	v2f_sub(&extent.max, &extent.min, &dimension);

	array coords;
	array_init(&coords, sizeof(v2f));
	array_reserve(&coords, n);
	for (const v2f *vn=v+n; v!=vn; ++v)
	{
		v2f tex_coord;
		v2f_sub(v, &extent.min, &tex_coord);
		v2f_div(&tex_coord, &dimension, &tex_coord);
		array_append(&coords, &tex_coord);
	}
	vlt_mesh_init(tex_coords, coords.data, n);
	array_destroy(&coords);
}

void vlt_texture_bind(const vlt_texture *tex)
{
	glBindTexture(GL_TEXTURE_2D, tex->handle);
}

void vlt_texture_unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

