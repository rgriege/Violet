#include "violet/plugins/graphics/shader/ShaderCache.h"

#include "violet/plugins/graphics/shader/Shader.h"

#include <vector>

using namespace Violet;

namespace ShaderCacheNamespace
{
	std::vector<std::shared_ptr<ShaderProgram>> ms_shaders;
};

using namespace ShaderCacheNamespace;

std::shared_ptr<ShaderProgram> ShaderCache::fetch(const char * name)
{
	auto ptr = ShaderProgram::create(name);
	ms_shaders.emplace_back(ptr.release());
	return ms_shaders.back();
}