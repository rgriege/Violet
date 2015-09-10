#ifndef VIOLET_Font_H
#define VIOLET_Font_H

#include "violet/utility/ResourceCache.h"

#include "violet/graphics/Mesh.h"
#include "violet/graphics/texture/Texture.h"

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

		static std::unique_ptr<Font> load(const char * filename, uint32 size);
		typedef ResourceCache<Font, const char *, uint32> Cache;
		static Cache & getCache();

	public:

		void render(std::string const & str, ShaderProgram & program);
		const char * getFilename() const;
		uint32 getSize() const;

	private:

		Font(const char * filename, uint32 size, std::map<char, Glyph> && glyphs, uint32 spaceWidth);

		Font(const Font &) = delete;
		Font & operator=(const Font &) = delete;

	private:

		const std::string m_filename;
		const uint32 m_size;
		std::map<char, Glyph> m_glyphs;
		const uint32 m_spaceWidth;
	};
}

#endif