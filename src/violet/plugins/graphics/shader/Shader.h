#ifndef SHADER_H
#define SHADER_H

#include "violet/core/utility/ResourceCache.h"
#include "violet/plugins/graphics/Config.h"

#include <memory>

namespace Violet
{
	class VIOLET_GRAPHICS_API Shader
	{
	public:

		friend class ShaderProgram;

		enum Type
		{
			Vertex,
			Fragment
		};

	public:

		static std::unique_ptr<Shader> create(const char * filename, Type type);

	public:

		~Shader();

		const char * getFilename() const;

	private:

		Shader(const char * filename, uint32 handle);

	private:

		const std::string m_filename;
		const uint32 m_handle;
	};

	class VIOLET_GRAPHICS_API ShaderProgram
	{
	public:

		static std::unique_ptr<ShaderProgram> load(const char * name);
		static std::unique_ptr<ShaderProgram> load(const char * vertexShaderFilename, const char * fragmentShaderFilename);
		static std::unique_ptr<ShaderProgram> create(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
		typedef ResourceCache<ShaderProgram, &ShaderProgram::load> Cache;
		static Cache & getCache();
		static void bind(const ShaderProgram & program);
		static void unbind();

	public:

		ShaderProgram(uint32 handle, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
		~ShaderProgram();

		std::string getName() const;
		int getAttributeLocation(const char * name);
		int getUniformLocation(const char * name);

	private:

		const uint32 m_handle;
		const std::shared_ptr<Shader> m_vertexShader;
		const std::shared_ptr<Shader> m_fragmentShader;
	};
}

#endif