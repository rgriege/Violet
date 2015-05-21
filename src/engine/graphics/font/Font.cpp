// ============================================================================

#include "engine/graphics/font/Font.h"

#include "engine/Defines.h"
#include "engine/utility/Guard.h"
#include "engine/graphics/shader/Shader.h"

#include <iostream>
#include <map>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace Violet;

// ============================================================================

namespace FontNamespace
{
	FT_Library ms_freetypeLibrary;

	uint32 ms_fontImageSize = 60;
};

using namespace FontNamespace;

// ============================================================================

Font::Glyph::Glyph(const uint32 vertexArrayBuffer, Texture && texture, Mesh && mesh, Mesh && texCoords, const uint32 advance) :
	m_vertexArrayBuffer(vertexArrayBuffer),
	m_texture(std::move(texture)),
	m_mesh(std::move(mesh)),
	m_texCoords(std::move(texCoords)),
	m_advance(advance)
{
}

// ----------------------------------------------------------------------------

Font::Glyph::Glyph(Glyph && other) :
	m_vertexArrayBuffer(other.m_vertexArrayBuffer),
	m_texture(std::move(other.m_texture)),
	m_mesh(std::move(other.m_mesh)),
	m_texCoords(std::move(other.m_texCoords)),
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

int Font::Glyph::getAdvance() const
{
	return m_advance;
}

// ============================================================================

std::unique_ptr<Font> Font::load(const char * filename)
{
	if (ms_freetypeLibrary == nullptr)
	{
		auto ftError = FT_Init_FreeType(&ms_freetypeLibrary);
		if (ftError != FT_Err_Ok)
		{
			std::cout << "FT_Init_FreeType error: " << ftError << std::endl;
			return nullptr;
		}
	}

	FT_Face face;
	auto ftError = FT_New_Face(ms_freetypeLibrary, filename, 0, &face);
	if (ftError != FT_Err_Ok)
	{
		std::cout << "FT_New_Face error: " << ftError << std::endl;
		return nullptr;
	}

	ftError = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (ftError != FT_Err_Ok)
	{
		std::cout << "FT_Select_Charmap error: " << ftError << std::endl;
		return nullptr;
	}

	ftError = FT_Set_Char_Size(face, 0, ms_fontImageSize * 64, 0, 0);
	if (ftError != FT_Err_Ok)
	{
		std::cout << "FT_Set_Char_Size error: " << ftError << std::endl;
		return nullptr;
	}

	std::map<char, Glyph> glyphs;
	uint32 spaceWidth = ms_fontImageSize;

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
			std::cout << "FT_Load_Glyph error: " << ftError << std::endl;
			return nullptr;
		}

		const FT_Bitmap bitmap = face->glyph->bitmap;
		if (bitmap.buffer != nullptr)
		{
			const uint32 texHeight = static_cast<uint32>(pow(2, ceil(log2(bitmap.rows))));
			const uint32 texWidth = static_cast<uint32>(pow(2, ceil(log2(bitmap.width))));
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

			Texture texture(texWidth, texHeight, pixels, GL_ALPHA);

			const float width = static_cast<float>(bitmap.width);
			const float height = static_cast<float>(bitmap.rows);
			Mesh mesh({
				{ 0, 0 },
				{ 0, height },
				{ width, height },
				{ width, 0 }
			});

			const float uMaxTexCoord = static_cast<float>(bitmap.width) / texWidth;
			const float vMaxTexCoord = static_cast<float>(bitmap.rows) / texHeight;
			Mesh texCoords({
				{ 0.f, vMaxTexCoord },
				{ 0.f, 0.f },
				{ uMaxTexCoord, 0.f },
				{ uMaxTexCoord, vMaxTexCoord }
			});

			glyphs.emplace(static_cast<char>(charcode), Glyph(vertexArrayBuffer, std::move(texture), std::move(mesh), std::move(texCoords), face->glyph->advance.x >> 6));

			delete[] pixels;
		}
		else if (charcode == 32)
			spaceWidth = face->glyph->advance.x >> 6;
		else
			std::cout << "Charcode has no bitmap: " << charcode << std::endl;

		charcode = FT_Get_Next_Char(face, charcode, &glyphIndex);
	}

	ftError = FT_Done_Face(face);
	if (ftError != FT_Err_Ok)
	{
		std::cout << "FT_Done_Face error: " << ftError << std::endl;
		return nullptr;
	}

	return std::unique_ptr<Font>(new Font(filename, std::move(glyphs), spaceWidth));
}

// ----------------------------------------------------------------------------

Font::Cache & Font::getCache()
{
	static Cache s_cache;
	return s_cache;
}

// ----------------------------------------------------------------------------

uint32 Font::getFontImageSize()
{
	return ms_fontImageSize;
}

// ============================================================================

void Font::render(std::string const & str, ShaderProgram & program)
{
	float xOffset = 0;
	// Only God knows why ranged for doesn't work here...
	for (uint32 i = 0, end = str.size(); i < end; ++i)
	{
		char character = str[i];
		if (character == ' ')
			xOffset += m_spaceWidth;
		else
		{
			auto const it = m_glyphs.find(character);
			if (it != m_glyphs.end())
			{
				const GLint advanceAttrib = program.getUniformLocation("advance");
				glUniform1f(advanceAttrib, xOffset);
				it->second.render(program);
				xOffset += it->second.getAdvance();
			}
			else
				std::cout << "unknown character: " << character << std::endl;
		}
	}
}

// ----------------------------------------------------------------------------

const char * Font::getFilename() const
{
	return m_filename.c_str();
}

// ============================================================================

Font::Font(const char * filename, std::map<char, Glyph> && glyphs, const uint32 spaceWidth) :
	m_filename(filename),
	m_glyphs(std::move(glyphs)),
	m_spaceWidth(spaceWidth)
{
}

// ============================================================================