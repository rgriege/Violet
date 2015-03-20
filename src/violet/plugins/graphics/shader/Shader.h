#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <memory>

namespace Violet
{
	class Shader
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

	private:

		Shader(GLuint handle);

	private:

		const GLuint m_handle;
	};

	class ShaderProgram
	{
	public:

		class Guard
		{
		public:

			Guard(GLuint handle);
			~Guard();
		};

	public:

		static std::unique_ptr<ShaderProgram> create(const char * name);
		static std::unique_ptr<ShaderProgram> create(const char * vertexShaderFilename, const char * fragmentShaderFilename);
		static std::unique_ptr<ShaderProgram> create(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);

	public:

		ShaderProgram(GLuint handle, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
		~ShaderProgram();

		int getAttributeLocation(const char * name);
		int getUniformLocation(const char * name);

		Guard use();

	private:

		const GLuint m_handle;
		const std::shared_ptr<Shader> m_vertexShader;
		const std::shared_ptr<Shader> m_fragmentShader;
	};
}

#endif