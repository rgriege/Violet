// ============================================================================

#include <algorithm>
#include <map>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "violet/core/defines.h"
#include "violet/log/log.h"
#include "violet/graphics/font/font.h"
#include "violet/graphics/shader/shader.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/guard.h"

using namespace vlt;

// ============================================================================

FT_Library g_freetype_lib;

// ============================================================================

Font::Glyph::Glyph(const u32 _vertex_array_buffer, Texture && _texture, Mesh && _mesh, Mesh && _tex_coords, const v2 & _offset, const u32 _advance) :
	vertex_array_buffer(_vertex_array_buffer),
	texture(std::move(_texture)),
	mesh(std::move(_mesh)),
	tex_coords(std::move(_tex_coords)),
	offset(_offset),
	advance(_advance)
{
}

// ----------------------------------------------------------------------------

Font::Glyph::Glyph(Glyph && other) :
	vertex_array_buffer(other.vertex_array_buffer),
	texture(std::move(other.texture)),
	mesh(std::move(other.mesh)),
	tex_coords(std::move(other.tex_coords)),
	offset(other.offset),
	advance(other.advance)
{
	other.vertex_array_buffer = 0;
}

// ----------------------------------------------------------------------------

Font::Glyph::~Glyph()
{
	if (vertex_array_buffer != 0)
		glDeleteVertexArrays(1, &vertex_array_buffer);
}

// ----------------------------------------------------------------------------

void Font::Glyph::render(Shader_Program & program) const
{
	glBindVertexArray(vertex_array_buffer);
	const Guard<Texture> tex_guard(texture);

	const Guard<Mesh> mesh_guard(mesh);
	const GLint pos_attrib = program.get_attrib_loc("position");
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(pos_attrib);

	const Guard<Mesh> texCoordGuard(tex_coords);
	const GLint tex_coord_attrib = program.get_attrib_loc("texCoord");
	glVertexAttribPointer(tex_coord_attrib, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(tex_coord_attrib);

	glDrawArrays(GL_TRIANGLE_FAN, 0, mesh.size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

u32 Font::Glyph::get_advance() const
{
	return advance;
}

// ----------------------------------------------------------------------------

const v2 & Font::Glyph::get_offset() const
{
	return offset;
}

// ============================================================================

std::unique_ptr<Font> Font::load(const char * const filename, const u32 size)
{
	if (g_freetype_lib == nullptr)
	{
		auto ftError = FT_Init_FreeType(&g_freetype_lib);
		if (ftError != FT_Err_Ok)
		{
			log(Formatted_String<64>().sprintf("FT_Init_FreeType error: %d", ftError));
			return nullptr;
		}
	}

	FT_Face face;
	auto ftError = FT_New_Face(g_freetype_lib, filename, 0, &face);
	if (ftError != FT_Err_Ok)
	{
		log(Formatted_String<64>().sprintf("FT_New_Face error: %d", ftError));
		return nullptr;
	}

	ftError = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (ftError != FT_Err_Ok)
	{
		log(Formatted_String<64>().sprintf("FT_Select_Charmap error: %d", ftError));
		return nullptr;
	}

	ftError = FT_Set_Char_Size(face, 0, size * 64, 0, 0);
	if (ftError != FT_Err_Ok)
	{
		log(Formatted_String<64>().sprintf("FT_Set_Char_Size error: %d", ftError));
		return nullptr;
	}

	std::map<char, Glyph> glyphs;
	u32 spaceWidth = size;

	FT_ULong charcode;
	FT_UInt GlyphIndex;
	charcode = FT_Get_First_Char(face, &GlyphIndex);
	while (GlyphIndex != 0)
	{
		if (charcode > 127)
		{
			charcode = FT_Get_Next_Char(face, charcode, &GlyphIndex);
			continue;
		}

		ftError = FT_Load_Glyph(face, GlyphIndex, FT_LOAD_RENDER);
		if (ftError != FT_Err_Ok)
		{
			log(Formatted_String<64>().sprintf("FT_Load_Glyph error: ", ftError));
			return nullptr;
		}

		const FT_Bitmap bitmap = face->glyph->bitmap;
		if (bitmap.buffer != nullptr)
		{
			const u32 texHeight = static_cast<u32>(pow(2, ceil(log2(bitmap.rows))));
			const u32 texWidth = std::max(4u, static_cast<u32>(pow(2, ceil(log2(bitmap.width))))); // For some reason 2 doesn't work...
			u8 * pixels = new u8[texHeight * texWidth];

			for (int i = 0; i < bitmap.rows; ++i)
			{
				memcpy(pixels + i * texWidth, bitmap.buffer + i * bitmap.width, bitmap.width);
				memset(pixels + i * texWidth + bitmap.width, 0, texWidth - bitmap.width);
			}
			for (u32 i = bitmap.rows; i < texHeight; ++i)
				memset(pixels + i * texWidth, 0, texWidth);

			GLuint vertexArrayBuffer = 0;
			glGenVertexArrays(1, &vertexArrayBuffer);
			glBindVertexArray(vertexArrayBuffer);

			Texture texture(filename, texWidth, texHeight, pixels, GL_ALPHA);

			const r32 width = static_cast<r32>(texWidth);
			const r32 height = static_cast<r32>(texHeight);
			Mesh m({
				{ 0, 0 },
				{ 0, height },
				{ width, height },
				{ width, 0 }
			});
			Mesh texCoords({
				{ 0.f, 1.f },
				{ 0.f, 0.f },
				{ 1.f, 0.f },
				{ 1.f, 1.f }
			});

			glyphs.emplace(static_cast<char>(charcode), Glyph(vertexArrayBuffer, std::move(texture), std::move(m), std::move(texCoords), v2(static_cast<r32>(face->glyph->bitmap_left), static_cast<r32>(face->glyph->bitmap_top - static_cast<int>(texHeight))), face->glyph->advance.x >> 6));

			delete[] pixels;
		}
		else if (charcode == 32)
			spaceWidth = face->glyph->advance.x >> 6;
		else
			log(Formatted_String<64>().sprintf("Charcode has no bitmap: %lu", charcode));

		charcode = FT_Get_Next_Char(face, charcode, &GlyphIndex);
	}

	ftError = FT_Done_Face(face);
	if (ftError != FT_Err_Ok)
	{
		log(Formatted_String<64>().sprintf("FT_Done_Face error: %d", ftError));
		return nullptr;
	}

	return std::unique_ptr<Font>(new Font(filename, size, std::move(glyphs), spaceWidth));
}

// ----------------------------------------------------------------------------

Font::Cache & Font::get_cache()
{
	static std::unique_ptr<Font>(*s_loader)(const char *, u32) = &Font::load;
	static Cache s_cache(s_loader);
	return s_cache;
}

// ============================================================================

void Font::render(std::string const & str, Shader_Program & program)
{
	r32 xOffset = 0;
	std::vector<std::pair<const Glyph &, v2>> glyph_offsets;
	// Only God knows why ranged for doesn't work here...
	for (u32 i = 0, end = str.size(); i < end; ++i)
	{
		char character = str[i];
		if (character == ' ')
			xOffset += space_width;
		else
		{
			const auto it = glyphs.find(character);
			if (it != glyphs.end())
			{
				glyph_offsets.emplace_back(it->second, it->second.get_offset() + v2(xOffset, 0.f));
				xOffset += it->second.get_advance();
			}
			else
				log(Formatted_String<64>().sprintf("unknown character: '%x'", character));
		}
	}

	r32 const halfWidth = xOffset / 2.f;
	for (const auto & glyph_offset : glyph_offsets)
	{
		// looks bad if offset not rounded
		const GLint offsetAttrib = program.get_uniform_loc("offset");
		glUniform2f(offsetAttrib, std::round(glyph_offset.second.x - halfWidth), glyph_offset.second.y);
		glyph_offset.first.render(program);
	}
}

// ----------------------------------------------------------------------------

const char * Font::get_filename() const
{
	return filename.c_str();
}

// ----------------------------------------------------------------------------

u32 Font::get_size() const
{
	return size;
}

// ============================================================================

Font::Font(const char * _filename, const u32 _size, std::map<char, Glyph> && _glyphs, const u32 _space_width) :
	filename(_filename),
	size(_size),
	glyphs(std::move(_glyphs)),
	space_width(_space_width)
{
}

// ============================================================================
