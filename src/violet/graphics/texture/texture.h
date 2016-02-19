#ifndef VIOLET_TEXTURE_H
#define VIOLET_TEXTURE_H

#include <iosfwd>
#include <memory>

#include "violet/core/defines.h"
#include "violet/utility/resource_cache.h"

namespace vlt
{
	struct VIOLET_API texture
	{
	public:

		static std::unique_ptr<texture> load_png(const char * filename);

		static void bind(const texture & texture);
		static void unbind();

		typedef resource_cache<texture, const char *> Cache;
		static Cache & get_cache();

	public:

		texture(std::string filename, u32 width, u32 height, const void * data, u32 format);
		texture(texture && other);
		~texture();

		const std::string & get_name() const;

	private:

		texture(const texture & other) = delete;
		texture & operator=(const texture & other) = delete;

	private:

		std::string m_filename;
		u32 m_handle;
	};
}

#endif