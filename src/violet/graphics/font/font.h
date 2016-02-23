#ifndef VIOLET_FONT_H
#define VIOLET_FONT_H

#include <map>
#include <memory>

#include "violet/graphics/mesh.h"
#include "violet/graphics/texture/texture.h"
#include "violet/utility/resource_cache.h"

namespace vlt
{
	struct Shader_Program;

	struct VIOLET_API Font
	{
		struct Glyph
		{
			u32 vertex_array_buffer;
			Texture texture;
			Mesh mesh;
			Mesh tex_coords;
			const v2 offset;
			const u32 advance;

			Glyph(u32 vertex_array_buffer, Texture && texture, Mesh && mesh, Mesh && tex_coords, const v2 & offset, u32 advance);
			Glyph(const Glyph & other) = delete;
			Glyph & operator=(const Glyph & other) = delete;
			Glyph(Glyph && other);
			~Glyph();

			void render(Shader_Program & program) const;
			const v2 & get_offset() const;
			u32 get_advance() const;
		};

		const std::string filename;
		const u32 size;
		std::map<char, Glyph> glyphs;
		const u32 space_width;

	public:

		static std::unique_ptr<Font> load(const char * filename, u32 size);
		typedef Resource_Cache<Font, const char *, u32> Cache;
		static Cache & get_cache();

	public:

		void render(std::string const & str, Shader_Program & program);
		const char * get_filename() const;
		u32 get_size() const;

	private:

		Font(const char * filename, u32 size, std::map<char, Glyph> && glyphs, u32 space_width);
		Font(const Font &) = delete;
		Font & operator=(const Font &) = delete;
	};
}

#endif
