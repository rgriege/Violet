// ============================================================================

#include "violet/graphics/font/Font.h"

#include "violet/Defines.h"
#include "violet/log/Log.h"
#include "violet/graphics/shader/Shader.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/Guard.h"

#include <algorithm>
#include <map>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace Violet;

// ============================================================================

namespace FontNamespace
{
	FT_Library ms_freetypeLibrary;
};

using namespace FontNamespace;

// ============================================================================

Font::Glyph::Glyph(const uint32 vertexArrayBuffer, Texture && texture, Mesh && mesh, Mesh && texCoords, const Vec2f & offset, const uint32 advance) :
	m_vertexArrayBuffer(vertexArrayBuffer),
	m_texture(std::move(texture)),
	m_mesh(std::move(mesh)),
	m_texCoords(std::move(texCoords)),
	m_offset(offset),
	m_advance(advance)
{
}

// ----------------------------------------------------------------------------

Font::Glyph::Glyph(Glyph && other) :
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

Font::Glyph::~Glyph()
{
	if (m_vertexArrayBuffer != 0)
		glDeleteVertexArrays(1, &m_vertexArrayBuffer);
}

// ----------------------------------------------------------------------------

void Font::Glyph::render(ShaderProgram & program) const
{
	glBindVertexArray(m_vertexArrayBuffer);
	const Guard<Texture> textureGuard(m_texture);

	const Guard<Mesh> meshGuard(m_mesh);
	const GLint positionAttribute = program.getAttributeLocation("position");
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	const Guard<Mesh> texCoordGuard(m_texCoords);
	const GLint texCoordAttribute = program.getAttributeLocation("texCoord");
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(texCoordAttribute);

	glDrawArrays(GL_TRIANGLE_FAN, 0, m_mesh.m_size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

uint32 Font::Glyph::getAdvance() const
{
	return m_advance;
}

// ----------------------------------------------------------------------------

const Vec2f & Font::Glyph::getOffset() const
{
	return m_offset;
}

// ============================================================================

std::unique_ptr<Font> Font::load(const char * const filename, const uint32 size)
{
	if (ms_freetypeLibrary == nullptr)
	{
		auto ftError = FT_Init_FreeType(&ms_freetypeLibrary);
		if (ftError != FT_Err_Ok)
		{
			Log::log(FormattedString<64>().sprintf("FT_Init_FreeType error: %d", ftError));
			return nullptr;
		}
	}

	FT_Face face;
	auto ftError = FT_New_Face(ms_freetypeLibrary, filename, 0, &face);
	if (ftError != FT_Err_Ok)
	{
		Log::log(FormattedString<64>().sprintf("FT_New_Face error: %d", ftError));
		return nullptr;
	}

	ftError = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (ftError != FT_Err_Ok)
	{
		Log::log(FormattedString<64>().sprintf("FT_Select_Charmap error: %d", ftError));
		return nullptr;
	}

	ftError = FT_Set_Char_Size(face, 0, size * 64, 0, 0);
	if (ftError != FT_Err_Ok)
	{
		Log::log(FormattedString<64>().sprintf("FT_Set_Char_Size error: %d", ftError));
		return nullptr;
	}

	std::map<char, Glyph> glyphs;
	uint32 spaceWidth = size;

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
			Log::log(FormattedString<64>().sprintf("FT_Load_Glyph error: ", ftError));
			return nullptr;
		}

		const FT_Bitmap bitmap = face->glyph->bitmap;
		if (bitmap.buffer != nullptr)
		{
			const uint32 texHeight = static_cast<uint32>(pow(2, ceil(log2(bitmap.rows))));
			const uint32 texWidth = std::max(4u, static_cast<uint32>(pow(2, ceil(log2(bitmap.width))))); // For some reason 2 doesn't work...
			uint8 * pixels = new uint8[texHeight * texWidth];

			for (int i = 0; i < bitmap.rows; ++i)
			{
				memcpy(pixels + i * texWidth, bitmap.buffer + i * bitmap.width, bitmap.width);
				memset(pixels + i * texWidth + bitmap.width, 0, texWidth - bitmap.width);
			}
			for (uint32 i = bitmap.rows; i < texHeight; ++i)
				memset(pixels + i * texWidth, 0, texWidth);

			GLuint vertexArrayBuffer = 0;
			glGenVertexArrays(1, &vertexArrayBuffer);
			glBindVertexArray(vertexArrayBuffer);

			Texture texture(filename, texWidth, texHeight, pixels, GL_ALPHA);

			const float width = static_cast<float>(texWidth);
			const float height = static_cast<float>(texHeight);
			Mesh mesh({
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

			glyphs.emplace(static_cast<char>(charcode), Glyph(vertexArrayBuffer, std::move(texture), std::move(mesh), std::move(texCoords), Vec2f(static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top - static_cast<int>(texHeight))), face->glyph->advance.x >> 6));

			delete[] pixels;
		}
		else if (charcode == 32)
			spaceWidth = face->glyph->advance.x >> 6;
		else
			Log::log(FormattedString<64>().sprintf("Charcode has no bitmap: %lu", charcode));

		charcode = FT_Get_Next_Char(face, charcode, &glyphIndex);
	}

	ftError = FT_Done_Face(face);
	if (ftError != FT_Err_Ok)
	{
		Log::log(FormattedString<64>().sprintf("FT_Done_Face error: %d", ftError));
		return nullptr;
	}

	return std::unique_ptr<Font>(new Font(filename, size, std::move(glyphs), spaceWidth));
}

// ----------------------------------------------------------------------------

Font::Cache & Font::getCache()
{
	static std::unique_ptr<Font>(*s_loader)(const char *, uint32) = &Font::load;
	static Cache s_cache(s_loader);
	return s_cache;
}

// ============================================================================

void Font::render(std::string const & str, ShaderProgram & program)
{
	float xOffset = 0;
	std::vector<std::pair<const Glyph &, Vec2f>> glyphOffsets;
	// Only God knows why ranged for doesn't work here...
	for (uint32 i = 0, end = str.size(); i < end; ++i)
	{
		char character = str[i];
		if (character == ' ')
			xOffset += m_spaceWidth;
		else
		{
			const auto it = m_glyphs.find(character);
			if (it != m_glyphs.end())
			{
				glyphOffsets.emplace_back(it->second, it->second.getOffset() + Vec2f(xOffset, 0.f));
				xOffset += it->second.getAdvance();
			}
			else
				Log::log(FormattedString<64>().sprintf("unknown character: '%x'", character));
		}
	}

	float const halfWidth = xOffset / 2.f;
	for (const auto & glyphOffset : glyphOffsets)
	{
		// looks bad if offset not rounded
		const GLint offsetAttrib = program.getUniformLocation("offset");
		glUniform2f(offsetAttrib, std::round(glyphOffset.second.x - halfWidth), glyphOffset.second.y);
		glyphOffset.first.render(program);
	}
}

// ----------------------------------------------------------------------------

const char * Font::getFilename() const
{
	return m_filename.c_str();
}

// ----------------------------------------------------------------------------

uint32 Font::getSize() const
{
	return m_size;
}

// ============================================================================

Font::Font(const char * filename, const uint32 size, std::map<char, Glyph> && glyphs, const uint32 spaceWidth) :
	m_filename(filename),
	m_size(size),
	m_glyphs(std::move(glyphs)),
	m_spaceWidth(spaceWidth)
{
}

// ============================================================================
