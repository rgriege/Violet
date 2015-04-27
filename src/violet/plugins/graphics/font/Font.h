#ifndef FONT_H
#define FONT_H

#include "violet/core/utility/ResourceCache.h"

#include "violet/plugins/graphics/Mesh.h"
#include "violet/plugins/graphics/texture/Texture.h"

#include <memory>
#include <map>

namespace Violet
{
	class ShaderProgram;

	class Font
	{
	private:

		class Glyph
		{
		public:

			Glyph(uint32 vertexArrayBuffer, Texture && texture, Mesh && mesh, Mesh && texCoords, uint32 advance);
			Glyph(Glyph && other);
			~Glyph();

			void render(ShaderProgram & program) const;
			int getAdvance() const;

		private:

			Glyph(const Glyph & other) = delete;
			Glyph & operator=(const Glyph & other) = delete;

		private:

			uint32 m_vertexArrayBuffer;
			Texture m_texture;
			Mesh m_mesh;
			Mesh m_texCoords;
			uint32 m_advance;
		};

	public:

		static std::unique_ptr<Font> load(const char * filename);
		typedef ResourceCache<Font, &Font::load> Cache;
		static Cache & getCache();
		static uint32 getFontImageSize();

	public:

		void render(std::string const & str, ShaderProgram & program);
		const char * getFilename() const;

	private:

		Font(const char * filename, std::map<char, Glyph> && glyphs, uint32 spaceWidth);

	private:

		const std::string m_filename;
		std::map<char, Glyph> m_glyphs;
		uint32 m_spaceWidth;
	};
}

#endif