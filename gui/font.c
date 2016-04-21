#include <assert.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <math.h>
#include <string.h>

#include "violet/math/math.h"
#include "violet/math/v2i.h"
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
	array_map_init(&f->glyphs, sizeof(char), sizeof(vlt_glyph));
	array_reserve(&f->glyphs.pairs, 128);


	FT_ULong charcode;
	FT_UInt glyph_idx;
	charcode = FT_Get_First_Char(face, &glyph_idx);
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
			log_write("FT_Load_Glyph error: ", err);
			continue;
		}

		const FT_Bitmap bitmap = face->glyph->bitmap;
		if (bitmap.buffer)
		{
			vlt_glyph * glyph = array_map_insert_null(&f->glyphs, &charcode);
			const u32 tex_height = pow(2, ceil(log2(bitmap.rows)));
			const u32 tex_width = max(4u, pow(2, ceil(log2(bitmap.width)))); // For some reason 2 doesn't work...
			u8 * pixels = malloc(tex_height * tex_width);

			for (int i = 0; i < bitmap.rows; ++i)
			{
				memcpy(pixels + i * tex_width, bitmap.buffer + i * bitmap.width, bitmap.width);
				memset(pixels + i * tex_width + bitmap.width, 0, tex_width - bitmap.width);
			}
			for (u32 i = bitmap.rows; i < tex_height; ++i)
				memset(pixels + i * tex_width, 0, tex_width);

			glGenVertexArrays(1, &glyph->vao);
			glBindVertexArray(glyph->vao);

			vlt_texture_init(&glyph->texture, tex_width, tex_height,
				GL_RED, pixels);

			array vertices;
			array_init(&vertices, sizeof(v2));
			array_reserve(&vertices, 4);
			const v2 vert0 = { .x=0, .y=0 };
			array_append(&vertices, &vert0);
			const v2 vert1 = { .x=0, .y=tex_height };
			array_append(&vertices, &vert1);
			const v2 vert2 = { .x=tex_width, .y=tex_height };
			array_append(&vertices, &vert2);
			const v2 vert3 = { .x=tex_width, .y=0 };
			array_append(&vertices, &vert3);
			vlt_mesh_init(&glyph->mesh, &vertices);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(0);
			array_destroy(&vertices);

			
			array coords;
			array_init(&coords, sizeof(v2));
			array_reserve(&coords, 4);
			const v2 coord0 = { .x=0, .y=0 };
			array_append(&coords, &coord0);
			const v2 coord1 = { .x=0, .y=1 };
			array_append(&coords, &coord1);
			const v2 coord2 = { .x=1, .y=1 };
			array_append(&coords, &coord2);
			const v2 coord3 = { .x=1, .y=0 };
			array_append(&coords, &coord3);
			vlt_mesh_init(&glyph->tex_coords, &coords);
			glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(1);
			array_destroy(&coords);

			glBindVertexArray(0);

			glyph->offset.x = face->glyph->bitmap_left;
			glyph->offset.y = -face->glyph->bitmap_top;
			glyph->advance = face->glyph->advance.x >> 6;

			free(pixels);
		}
		else if (charcode == 32)
			f->space_width = face->glyph->advance.x >> 6;
		else
			log_write("Charcode has no bitmap: %lu", charcode);

		charcode = FT_Get_Next_Char(face, charcode, &glyph_idx);
	}
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

static r32 _width(vlt_font * f, const char * txt)
{
	r32 width = 0;
	for (u32 i = 0, end = strlen(txt); i < end; ++i)
	{
		const char character = txt[i];
		if (character == ' ')
			width += f->space_width;
		else
		{
			vlt_glyph * glyph = array_map_get(&f->glyphs, &character);
			if (glyph)
				width += glyph->advance;
			else
				log_write("unknown character: '%x'", character);
		}
	}
	return width;
}

void vlt_font_render(vlt_font * f, const char * txt, s32 _x, s32 y,
                     vlt_shader_program * p, font_align align)
{
	s32 off = 0;
	if (align == FONT_ALIGN_CENTER)
		off = _width(f, txt) / 2.f;
	else if (align == FONT_ALIGN_RIGHT)
		off = _width(f, txt);

	s32 x = _x;
	for (u32 i = 0, end = strlen(txt); i < end; ++i)
	{
		const char character = txt[i];
		if (character != ' ')
		{
			vlt_glyph * glyph = array_map_get(&f->glyphs, &character);
			if (glyph)
			{
				// looks bad if offset not rounded
				const GLint offset_attrib =
					vlt_shader_program_uniform(p, "offset");
				v2i offset = { .x = x, .y = y };
				v2i_add(&offset, &glyph->offset, &offset);
				glUniform2f(offset_attrib, offset.x - off, offset.y);
				_glyph_render(glyph, p);
				x += glyph->advance;
			}
		}
		else
			x += f->space_width;
	}
}

