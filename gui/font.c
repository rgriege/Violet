#include <assert.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <math.h>
#include <string.h>

#include "violet/math/r32.h"
#include "violet/math/s32.h"
#include "violet/structures/array_map.h"
#include "violet/utility/log.h"

#include "violet/gui/font.h"
#include "violet/gui/mesh.h"
#include "violet/gui/shader.h"
#include "violet/gui/texture.h"


// FT_Library

FT_Library g_freetype_lib = {0};

b8 vlt_font_install()
{
	assert(!g_freetype_lib);
	FT_Error err = FT_Init_FreeType(&g_freetype_lib);
	if (err != FT_Err_Ok)
	{
		log_write("FT_Init_FreeType error: %d", err);
		return false;
	}
	return true;
}

b8 vlt_font_uninstall()
{
	assert(g_freetype_lib);
	FT_Error err = FT_Done_FreeType(g_freetype_lib);
	if (err != FT_Err_Ok)
	{
		log_write("FT_Done_FreeType error: %d", err);
		return false;
	}
	return true;
}


typedef struct vlt_glyph
{
	u32 vao;
	vlt_texture texture;
	vlt_mesh mesh;
	vlt_mesh tex_coords;
	v2i offset;
	u32 advance;
} vlt_glyph;

static void _glyph_render(vlt_glyph * g, vlt_shader_program * p)
{
	glBindVertexArray(g->vao);

	vlt_texture_bind(&g->texture);

	glDrawArrays(GL_TRIANGLE_FAN, 0, g->mesh.sz);

	vlt_texture_unbind();

	glBindVertexArray(0);
}

static void _glyph_destroy(vlt_glyph * g)
{
	vlt_mesh_destroy(&g->mesh);
	vlt_mesh_destroy(&g->tex_coords);
	vlt_texture_destroy(&g->texture);
	glDeleteVertexArrays(1, &g->vao);
}


typedef struct vlt_font
{
	const char * filename;
	u32 sz;
	array_map glyphs; // char -> vlt_glyph
	u32 space_width;
	u32 line_dist;
} vlt_font;

vlt_font * vlt_font_create()
{
	return calloc(sizeof(vlt_font), 1);
}

void vlt_font_free(vlt_font * f)
{
	free(f);
}

b8 vlt_font_load(vlt_font * f, const char * filename, u32 sz)
{
	b8 retval = false;

	FT_Face face;
	FT_Error err = FT_New_Face(g_freetype_lib, filename, 0, &face);
	if (err != FT_Err_Ok)
	{
		log_write("FT_New_Face error: %d", err);
		return retval;
	}

	err = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (err != FT_Err_Ok)
	{
		log_write("FT_Select_Charmap error: %d", err);
		goto err_face;
	}

	err = FT_Set_Char_Size(face, 0, sz * 64, 0, 0);
	if (err != FT_Err_Ok)
	{
		log_write("FT_Set_Char_Size error: %d", err);
		goto err_face;
	}

	f->filename = filename;
	f->sz = sz;
	f->space_width = sz; // default
	f->line_dist = face->height >> 6;
	array_map_init(&f->glyphs, sizeof(char), sizeof(vlt_glyph));
	array_reserve(&f->glyphs.pairs, 128);


	FT_ULong charcode;
	FT_UInt glyph_idx;
	charcode = FT_Get_First_Char(face, &glyph_idx);
	u32 pixel_buf_sz = 32*32;
	u8 *pixels = malloc(pixel_buf_sz);
	while (glyph_idx != 0)
	{
		if (charcode > 127)
		{
			charcode = FT_Get_Next_Char(face, charcode, &glyph_idx);
			continue;
		}

		err = FT_Load_Glyph(face, glyph_idx, FT_LOAD_RENDER);
		if (err != FT_Err_Ok)
		{
			charcode = FT_Get_Next_Char(face, charcode, &glyph_idx);
			log_write("FT_Load_Glyph(%s, %u) error: 0x%x", filename, charcode, err);
			continue;
		}

		const FT_Bitmap bitmap = face->glyph->bitmap;
		if (bitmap.buffer)
		{
			vlt_glyph * glyph = array_map_insert_null(&f->glyphs, &charcode);
			// NOTE(rgriege): having issues with textures smaller than this
			const u32 tex_height = max(16u,pow(2,ceil(log2(bitmap.rows))));
			const u32 tex_width = max(16u,pow(2,ceil(log2(bitmap.width))));
			const u32 tex_sz = tex_width*tex_height;
			if (tex_sz > pixel_buf_sz)
			{
				free(pixels);
				pixels = malloc(tex_sz);
			}

			for (int i = 0; i < bitmap.rows; ++i)
			{
				memcpy(pixels+i*tex_width, bitmap.buffer+i*bitmap.width,
					bitmap.width);
				memset(pixels+i*tex_width+bitmap.width, 0,
					tex_width-bitmap.width);
			}
			memset(pixels+bitmap.rows*tex_width, 0,
				tex_width*(tex_height-bitmap.rows));

			glGenVertexArrays(1, &glyph->vao);
			glBindVertexArray(glyph->vao);

			vlt_texture_init(&glyph->texture, tex_width, tex_height,
				GL_RED, pixels);

			v2f vertices[4] = {
				{ .x=0, .y=0 },
				{ .x=0, .y=tex_height },
				{ .x=tex_width, .y=tex_height },
				{ .x=tex_width, .y=0 }
			};
			vlt_mesh_init(&glyph->mesh, vertices, 4);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(0);

			
			v2f coords[4] = {
				{ .x=0, .y=1 },
				{ .x=0, .y=0 },
				{ .x=1, .y=0 },
				{ .x=1, .y=1 }
			};
			vlt_mesh_init(&glyph->tex_coords, coords, 4);
			glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(1);

			glBindVertexArray(0);

			glyph->offset.x = face->glyph->bitmap_left;
			glyph->offset.y = face->glyph->bitmap_top - (r32)tex_height;
			glyph->advance = face->glyph->advance.x >> 6;
		}
		else if (charcode == 32)
			f->space_width = face->glyph->advance.x >> 6;
		else
			log_write("Charcode has no bitmap: %lu", charcode);

		charcode = FT_Get_Next_Char(face, charcode, &glyph_idx);
	}
	free(pixels);
	retval = true;

err_face:
	err = FT_Done_Face(face);
	if (err != FT_Err_Ok)
		log_write("FT_Done_Face error: %d", err);
	return retval;
}

void vlt_font_destroy(vlt_font * f)
{
	array_map_destroy_each(&f->glyphs, NULL, (void(*)(void*))_glyph_destroy);
}

static r32 _line_offset_x(vlt_font * f, const char * txt, font_align align)
{
	r32 factor = 0.f; // FONT_ALIGN_LEFT by default
	switch (align)
	{
	case FONT_ALIGN_LEFT:
		break;
	case FONT_ALIGN_CENTER:
		factor = 0.5f;
		break;
	case FONT_ALIGN_RIGHT:
		factor = 1.f;
		break;
	}
	r32 width = 0;
	for (u32 i = 0, end = strlen(txt); i < end; ++i)
	{
		const char character = txt[i];
		switch (character)
		{
		case ' ':
			width += f->space_width;
			break;
		case '\r':
			goto out;
			break;
		default:
		{
			vlt_glyph * glyph = array_map_get(&f->glyphs, &character);
			if (glyph)
				width += glyph->advance;
			else
				log_write("unknown character: '%u'", character);
		}
			break;
		}
	}
out:
	return width * factor;
}

void vlt_font_render(vlt_font * f, const char * txt, s32 _x, s32 _y,
                     vlt_shader_program * p, font_align align)
{
	s32 off = _line_offset_x(f, txt, align);
	v2i pos = { .x=_x, .y=_y };
	for (u32 i = 0, end = strlen(txt); i < end; ++i)
	{
		const char character = txt[i];
		switch (character)
		{
		case ' ':
			pos.x += f->space_width;
			break;

		case '\r':
			pos.y -= f->line_dist;
			pos.x = _x;
			if (i < end - 1)
				off = _line_offset_x(f, txt + i + 1, align);
			break;

		default:
		{
			vlt_glyph * glyph = array_map_get(&f->glyphs, &character);
			if (glyph)
			{
				// looks bad if offset not rounded
				const GLint offset_attrib =
					vlt_shader_program_uniform(p, "offset");
				v2i offset;
				v2i_add(&pos, &glyph->offset, &offset);
				glUniform2f(offset_attrib, offset.x - off, offset.y);
				_glyph_render(glyph, p);
				pos.x += glyph->advance;
			}
		}
			break;
		}
	}
}

