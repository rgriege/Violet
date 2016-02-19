// ============================================================================

#include "violet/graphics/font/font.h"

#include "violet/core/defines.h"
#include "violet/log/Log.h"
#include "violet/graphics/shader/shader.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/guard.h"

#include <algorithm>
#include <map>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace vlt;

// ============================================================================

namespace FontNamespace
{
	FT_Library ms_freetypeLibrary;
};

using namespace FontNamespace;

// ============================================================================

font::glyph::glyph(const u32 vertexArrayBuffer, texture && texture, mesh && m, mesh && texCoords, const v2 & offset, const u32 advance) :
	m_vertexArrayBuffer(vertexArrayBuffer),
	m_texture(std::move(texture)),
	m_mesh(std::move(m)),
	m_texCoords(std::move(texCoords)),
	m_offset(offset),
	m_advance(advance)
{
}

// ----------------------------------------------------------------------------

font::glyph::glyph(glyph && other) :
	m_vertexArrayBuffer(other.m_vertexArrayBuffer),
	m_texture(std::move(other.m_texture)),
	m_mesh(std::move(other.m_mesh)),
	m_texCoords(std::move(other.m_texCoords)),
	m_offset(other.m_offset),
	m_advance(other.m_advance)
{
	other.m_vertexArrayBuffer = 0;
}

// ----------------------------------------------------------------------------

font::glyph::~glyph()
{
	if (m_vertexArrayBuffer != 0)
		glDeleteVertexArrays(1, &m_vertexArrayBuffer);
}

// ----------------------------------------------------------------------------

void font::glyph::render(shader_program & program) const
{
	glBindVertexArray(m_vertexArrayBuffer);
	const guard<texture> textureGuard(m_texture);

	const guard<mesh> meshGuard(m_mesh);
	const GLint positionAttribute = program.getAttributeLocation("position");
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	const guard<mesh> texCoordGuard(m_texCoords);
	const GLint texCoordAttribute = program.getAttributeLocation("texCoord");
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(texCoordAttribute);

	glDrawArrays(GL_TRIANGLE_FAN, 0, m_mesh.m_size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

u32 font::glyph::get_advance() const
{
	return m_advance;
}

// ----------------------------------------------------------------------------

const v2 & font::glyph::get_offset() const
{
	return m_offset;
}

// ============================================================================

std::unique_ptr<font> font::load(const char * const filename, const u32 size)
{
	if (ms_freetypeLibrary == nullptr)
	{
		auto ftError = FT_Init_FreeType(&ms_freetypeLibrary);
		if (ftError != FT_Err_Ok)
		{
			log(formatted_string<64>().sprintf("FT_Init_FreeType error: %d", ftError));
			return nullptr;
		}
	}

	FT_Face face;
	auto ftError = FT_New_Face(ms_freetypeLibrary, filename, 0, &face);
	if (ftError != FT_Err_Ok)
	{
		log(formatted_string<64>().sprintf("FT_New_Face error: %d", ftError));
		return nullptr;
	}

	ftError = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (ftError != FT_Err_Ok)
	{
		log(formatted_string<64>().sprintf("FT_Select_Charmap error: %d", ftError));
		return nullptr;
	}

	ftError = FT_Set_Char_Size(face, 0, size * 64, 0, 0);
	if (ftError != FT_Err_Ok)
	{
		log(formatted_string<64>().sprintf("FT_Set_Char_Size error: %d", ftError));
		return nullptr;
	}

	std::map<char, glyph> glyphs;
	u32 spaceWidth = size;

	FT_ULong charcode;
	FT_UInt glyphIndex;
	charcode = FT_Get_First_Char(face, &glyphIndex);
	while (glyphIndex != 0)
	{
		if (charcode > 127)
		{
			charcode = FT_Get_Next_Char(face, charcode, &glyphIndex);
			continue;
		}

		ftError = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
		if (ftError != FT_Err_Ok)
		{
			log(formatted_string<64>().sprintf("FT_Load_Glyph error: ", ftError));
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

			texture texture(filename, texWidth, texHeight, pixels, GL_ALPHA);

			const r32 width = static_cast<r32>(texWidth);
			const r32 height = static_cast<r32>(texHeight);
			mesh m({
				{ 0, 0 },
				{ 0, height },
				{ width, height },
				{ width, 0 }
			});
			mesh texCoords({
				{ 0.f, 1.f },
				{ 0.f, 0.f },
				{ 1.f, 0.f },
				{ 1.f, 1.f }
			});

			glyphs.emplace(static_cast<char>(charcode), glyph(vertexArrayBuffer, std::move(texture), std::move(m), std::move(texCoords), v2(static_cast<r32>(face->glyph->bitmap_left), static_cast<r32>(face->glyph->bitmap_top - static_cast<int>(texHeight))), face->glyph->advance.x >> 6));

			delete[] pixels;
		}
		else if (charcode == 32)
			spaceWidth = face->glyph->advance.x >> 6;
		else
			log(formatted_string<64>().sprintf("Charcode has no bitmap: %lu", charcode));

		charcode = FT_Get_Next_Char(face, charcode, &glyphIndex);
	}

	ftError = FT_Done_Face(face);
	if (ftError != FT_Err_Ok)
	{
		log(formatted_string<64>().sprintf("FT_Done_Face error: %d", ftError));
		return nullptr;
	}

	return std::unique_ptr<font>(new font(filename, size, std::move(glyphs), spaceWidth));
}

// ----------------------------------------------------------------------------

font::Cache & font::get_cache()
{
	static std::unique_ptr<font>(*s_loader)(const char *, u32) = &font::load;
	static Cache s_cache(s_loader);
	return s_cache;
}

// ============================================================================

void font::render(std::string const & str, shader_program & program)
{
	r32 xOffset = 0;
	std::vector<std::pair<const glyph &, v2>> glyphOffsets;
	// Only God knows why ranged for doesn't work here...
	for (u32 i = 0, end = str.size(); i < end; ++i)
	{
		char character = str[i];
		if (character == ' ')
			xOffset += m_spaceWidth;
		else
		{
			const auto it = m_glyphs.find(character);
			if (it != m_glyphs.end())
			{
				glyphOffsets.emplace_back(it->second, it->second.get_offset() + v2(xOffset, 0.f));
				xOffset += it->second.get_advance();
			}
			else
				log(formatted_string<64>().sprintf("unknown character: '%x'", character));
		}
	}

	r32 const halfWidth = xOffset / 2.f;
	for (const auto & glyphOffset : glyphOffsets)
	{
		// looks bad if offset not rounded
		const GLint offsetAttrib = program.getUniformLocation("offset");
		glUniform2f(offsetAttrib, std::round(glyphOffset.second.x - halfWidth), glyphOffset.second.y);
		glyphOffset.first.render(program);
	}
}

// ----------------------------------------------------------------------------

const char * font::get_filename() const
{
	return m_filename.c_str();
}

// ----------------------------------------------------------------------------

u32 font::get_size() const
{
	return m_size;
}

// ============================================================================

font::font(const char * filename, const u32 size, std::map<char, glyph> && glyphs, const u32 spaceWidth) :
	m_filename(filename),
	m_size(size),
	m_glyphs(std::move(glyphs)),
	m_spaceWidth(spaceWidth)
{
}

// ============================================================================
