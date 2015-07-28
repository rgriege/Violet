#ifndef VIOLET_Texture_H
#define VIOLET_Texture_H

#include "engine/Defines.h"
#include "engine/utility/ResourceCache.h"

#include <memory>
#include <iosfwd>

namespace Violet
{
	class VIOLET_API Texture
	{
	public:

		static std::unique_ptr<Texture> loadPng(const char * filename);

		static void bind(const Texture & texture);
		static void unbind();

		typedef ResourceCache<Texture, &Texture::loadPng> Cache;
		static Cache & getCache();

	public:

		Texture(std::string filename, uint32 width, uint32 height, const void * data, uint32 format);
		Texture(Texture && other);
		~Texture();

		const std::string & getName() const;

	private:

		Texture(const Texture & other) = delete;
		Texture & operator=(const Texture & other) = delete;

	private:

		std::string m_filename;
		uint32 m_handle;
	};
}

#endif