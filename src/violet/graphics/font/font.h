#ifndef VIOLET_FONT_H
#define VIOLET_FONT_H

#include "violet/utility/resource_cache.h"

#include "violet/graphics/mesh.h"
#include "violet/graphics/texture/texture.h"

#include <memory>
#include <map>

namespace vlt
{
	struct shader_program;

	struct VIOLET_API font
	{
	private:

		struct glyph
		{
		public:

			glyph(u32 vertexArrayBuffer, texture && texture, mesh && _mesh, mesh && texCoords, const v2 & offset, u32 advance);
			glyph(glyph && other);
			~glyph();

			void render(shader_program & program) const;
			const v2 & get_offset() const;
			u32 get_advance() const;

		private:

			glyph(const glyph & other) = delete;
			glyph & operator=(const glyph & other) = delete;

		private:

			u32 m_vertexArrayBuffer;
			texture m_texture;
			mesh m_mesh;
			mesh m_texCoords;
			const v2 m_offset;
			const u32 m_advance;
		};

	public:

		static std::unique_ptr<font> load(const char * filename, u32 size);
		typedef resource_cache<font, const char *, u32> Cache;
		static Cache & get_cache();

	public:

		void render(std::string const & str, shader_program & program);
		const char * get_filename() const;
		u32 get_size() const;

	private:

		font(const char * filename, u32 size, std::map<char, glyph> && glyphs, u32 spaceWidth);

		font(const font &) = delete;
		font & operator=(const font &) = delete;

	private:

		const std::string m_filename;
		const u32 m_size;
		std::map<char, glyph> m_glyphs;
		const u32 m_spaceWidth;
	};
}

#endif