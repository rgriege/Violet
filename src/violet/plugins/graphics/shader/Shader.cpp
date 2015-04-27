#include "violet/plugins/graphics/shader/Shader.h"

#include "violet/core/Defines.h"
#include "violet/core/utility/Buffer.h"

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace Violet;

namespace ShaderNamespace
{
	std::map<GLuint, std::string> ms_shaderFilenames;
};

using namespace ShaderNamespace;

std::unique_ptr<Shader> Shader::create(const char * filename, Type type)
{
	std::ifstream stream(filename, std::ios_base::binary);
	if (!stream)
	{
		std::cout << "Could not open shader file " << filename << std::endl;
		return nullptr;
	}

	std::streampos const start = stream.tellg();
	stream.seekg(0, std::ios::end);
	uint32 const size = static_cast<uint32>(stream.tellg() - start);
	stream.seekg(0, std::ios::beg);
	HeapBuffer<char> buffer(size + 1);
	stream.read(buffer, size);
	buffer[size] = 0;

	GLuint const shader = glCreateShader(type == Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, (const GLchar **)&buffer, 0);

	glCompileShader(shader);
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char * log = static_cast<char *>(malloc(length));
		glGetShaderInfoLog(shader, length, nullptr, log);
		std::cout << "Compilation error in shader file " << filename << ":" << std::endl;
		std::cout << log;
		free(log);
		return nullptr;
	}

	ms_shaderFilenames[shader] = filename;
	return std::unique_ptr<Shader>(new Shader(filename, shader));
}

Shader::Shader(const char * filename, const GLuint handle) :
	m_filename(filename),
	m_handle(handle)
{
}

Shader::~Shader()
{
	glDeleteShader(m_handle);
}

const char * Shader::getFilename() const
{
	return m_filename.c_str();
}

std::unique_ptr<ShaderProgram> ShaderProgram::load(const char * name)
{
	std::string nameStr = name;
	return load((nameStr + ".vert").c_str(), (nameStr + ".frag").c_str());
}

std::unique_ptr<ShaderProgram> ShaderProgram::load(const char * vertexShaderFilename, const char * fragmentShaderFilename)
{
	auto vertexShader = Shader::create(vertexShaderFilename, Shader::Vertex);
	auto fragmentShader = Shader::create(fragmentShaderFilename, Shader::Fragment);
	
	return (vertexShader != nullptr && fragmentShader != nullptr) ? ShaderProgram::create(std::shared_ptr<Shader>(vertexShader.release()), std::shared_ptr<Shader>(fragmentShader.release())) : nullptr;
}

std::unique_ptr<ShaderProgram> ShaderProgram::create(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader)
{
	GLuint program = glCreateProgram();

	glAttachShader(program, vertexShader->m_handle);
	glAttachShader(program, fragmentShader->m_handle);

	glLinkProgram(program);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char * log = static_cast<char *>(malloc(length));
		glGetProgramInfoLog(program, length, nullptr, log);
		std::cout << "Link error in shader program using " << ms_shaderFilenames[vertexShader->m_handle] << " and " << ms_shaderFilenames[fragmentShader->m_handle] << ":" << std::endl;
		std::cout << log;
		free(log);
		return 0;
	}
	
	return std::unique_ptr<ShaderProgram>(new ShaderProgram(program, vertexShader, fragmentShader));
}

ShaderProgram::Cache & ShaderProgram::getCache()
{
	static Cache s_cache;
	return s_cache;
}

void ShaderProgram::bind(const ShaderProgram & program)
{
	glUseProgram(program.m_handle);
}

void ShaderProgram::unbind()
{
	glUseProgram(0);
}

ShaderProgram::ShaderProgram(const GLuint handle, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader) :
	m_handle(handle),
	m_vertexShader(std::move(vertexShader)),
	m_fragmentShader(std::move(fragmentShader))
{
}

ShaderProgram::~ShaderProgram()
{
	glDetachShader(m_handle, m_vertexShader->m_handle);
	glDetachShader(m_handle, m_fragmentShader->m_handle);
	glDeleteProgram(m_handle);
}

std::string ShaderProgram::getName() const
{
	return StringUtilities::left(m_vertexShader->getFilename(), '.');
}

int ShaderProgram::getAttributeLocation(const char * name)
{
	return glGetAttribLocation(m_handle, name);
}

int ShaderProgram::getUniformLocation(const char * name)
{
	return glGetUniformLocation(m_handle, name);
}
