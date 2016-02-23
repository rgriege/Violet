#ifndef VIOLET_SHADER_H
#define VIOLET_SHADER_H

#include "violet/utility/resource_cache.h"
#include "violet/core/defines.h"

#include <memory>

namespace vlt
{
	struct VIOLET_API Shader
	{
		const std::string filename;
		const u32 handle;

		enum Type
		{
			Vertex,
			Fragment
		};

		static std::unique_ptr<Shader> create(const char * filename, Type type);

		Shader(const char * filename, u32 handle);
		~Shader();
	};

	struct VIOLET_API Shader_Program
	{
		const u32 handle;
		const std::shared_ptr<Shader> vertex_shader;
		const std::shared_ptr<Shader> fragment_shader;

		static std::unique_ptr<Shader_Program> load(const char * name);
		static std::unique_ptr<Shader_Program> load(const char * vertex_shader_filename, const char * fragment_shader_filename);
		static std::unique_ptr<Shader_Program> create(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader);
		typedef Resource_Cache<Shader_Program, const char *> Cache;
		static Cache & get_cache();
		static void bind(const Shader_Program & program);
		static void unbind();

		Shader_Program(u32 handle, std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader);
		~Shader_Program();

		std::string get_name() const;
		int get_attrib_loc(const char * name);
		int get_uniform_loc(const char * name);
	};
}

#endif
