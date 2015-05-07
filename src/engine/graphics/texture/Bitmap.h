#ifndef VIOLET_Bitmap_H
#define VIOLET_Bitmap_H

#include "engine/utility/ResourceCache.h"

#include <memory>
#include <iosfwd>

namespace Violet
{
	class Bitmap
	{
	public:
		
		static std::unique_ptr<Bitmap> load(const char * filename);
		static std::unique_ptr<Bitmap> load(std::istream & stream);
		typedef ResourceCache<Bitmap, &Bitmap::load> Cache;
		static Cache & getCache();

	private:

		uint32 m_texture;
	};
}

#endif