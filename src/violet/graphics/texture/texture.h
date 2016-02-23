#ifndef VIOLET_TEXTURE_H
#define VIOLET_TEXTURE_H

#include <iosfwd>
#include <memory>

#include "violet/core/defines.h"
#include "violet/utility/resource_cache.h"

namespace vlt
{
	struct VIOLET_API Texture
	{
		std::string filename;
		u32 handle;

		static std::unique_ptr<Texture> load_png(const char * filename);

		static void bind(const Texture & texture);
		static void unbind();

		typedef Resource_Cache<Texture, const char *> Cache;
		static Cache & get_cache();

	public:

		Texture(std::string filename, u32 width, u32 height, const void * data, u32 format);
		Texture(const Texture & other) = delete;
		Texture & operator=(const Texture & other) = delete;
		Texture(Texture && other);
		~Texture();
	};
}

#endif
