#ifndef SHADER_CACHE_H
#define SHADER_CACHE_H

#include <memory>

namespace Violet
{
	class ShaderProgram;

	class ShaderCache
	{
	public:

		static std::shared_ptr<ShaderProgram> fetch(const char * name);
	};
}

#endif