#ifndef VIOLET_SHADER_H
#define VIOLET_SHADER_H

#include "violet/utility/resource_cache.h"
#include "violet/core/defines.h"

#include <memory>

namespace vlt
{
	struct VIOLET_API shader
	{
		friend struct shader_program;

		enum Type
		{
			Vertex,
			Fragment
		};

	public:

		static std::unique_ptr<shader> create(const char * filename, Type type);

	public:

		~shader();

		const char * get_filename() const;

	private:

		shader(const char * filename, u32 handle);

	private:

		const std::string m_filename;
		const u32 m_handle;
	};

	struct VIOLET_API shader_program
	{
		static std::unique_ptr<shader_program> load(const char * name);
		static std::unique_ptr<shader_program> load(const char * vertexShaderFilename, const char * fragmentShaderFilename);
		static std::unique_ptr<shader_program> create(std::shared_ptr<shader> vertexShader, std::shared_ptr<shader> fragmentShader);
		typedef resource_cache<shader_program, const char *> Cache;
		static Cache & get_cache();
		static void bind(const shader_program & program);
		static void unbind();

	public:

		shader_program(u32 handle, std::shared_ptr<shader> vertexShader, std::shared_ptr<shader> fragmentShader);
		~shader_program();

		std::string get_name() const;
		int getAttributeLocation(const char * name);
		int getUniformLocation(const char * name);

	private:

		const u32 m_handle;
		const std::shared_ptr<shader> m_vertexShader;
		const std::shared_ptr<shader> m_fragmentShader;
	};
}

#endif