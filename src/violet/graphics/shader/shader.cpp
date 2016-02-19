#include "violet/graphics/shader/shader.h"

#include "violet/core/defines.h"
#include "violet/log/log.h"
#include "violet/utility/buffer.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/string_utilities.h"

#include <GL/glew.h>
#include <fstream>
#include <map>
#include <string>

using namespace vlt;

namespace ShaderNamespace
{
	std::map<GLuint, std::string> ms_shaderFilenames;
};

using namespace ShaderNamespace;

std::unique_ptr<shader> shader::create(const char * filename, Type type)
{
	std::ifstream stream(filename, std::ios_base::binary);
	if (!stream)
	{
		log(formatted_string<128>().sprintf("Could not open shader file '%s'", filename));
		return nullptr;
	}

	std::streampos const start = stream.tellg();
	stream.seekg(0, std::ios::end);
	u32 const size = static_cast<u32>(stream.tellg() - start);
	stream.seekg(0, std::ios::beg);
	heap_buffer<char> buffer(size + 1);
	stream.read(buffer, size);
	buffer[size] = 0;

	GLuint const shader_handle = glCreateShader(type == Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(shader_handle, 1, (const GLchar **)&buffer, 0);

	glCompileShader(shader_handle);
	GLint compiled;
	glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &length);
		heap_buffer<char> log_buffer(length);
		glGetShaderInfoLog(shader_handle, length, nullptr, log_buffer);
		log(formatted_string<1024>().sprintf("Compilation error in shader file '%s': %s", filename, log_buffer.data));
		return nullptr;
	}

	ms_shaderFilenames[shader_handle] = filename;
	return std::unique_ptr<shader>(new shader(filename, shader_handle));
}

shader::shader(const char * filename, const GLuint handle) :
	m_filename(filename),
	m_handle(handle)
{
}

shader::~shader()
{
	glDeleteShader(m_handle);
}

const char * shader::get_filename() const
{
	return m_filename.c_str();
}

std::unique_ptr<shader_program> shader_program::load(const char * name)
{
	std::string nameStr = name;
	return load((nameStr + ".vert").c_str(), (nameStr + ".frag").c_str());
}

std::unique_ptr<shader_program> shader_program::load(const char * vertexShaderFilename, const char * fragmentShaderFilename)
{
	auto vertexShader = shader::create(vertexShaderFilename, shader::Vertex);
	auto fragmentShader = shader::create(fragmentShaderFilename, shader::Fragment);
	
	return (vertexShader != nullptr && fragmentShader != nullptr) ? shader_program::create(std::shared_ptr<shader>(vertexShader.release()), std::shared_ptr<shader>(fragmentShader.release())) : nullptr;
}

std::unique_ptr<shader_program> shader_program::create(std::shared_ptr<shader> vertexShader, std::shared_ptr<shader> fragmentShader)
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
		heap_buffer<char> log_buffer(length);
		glGetProgramInfoLog(program, length, nullptr, log_buffer);
		log(formatted_string<1024>().sprintf("Link error in shader program using '%s' and '%s': %s", ms_shaderFilenames[vertexShader->m_handle].c_str(), ms_shaderFilenames[fragmentShader->m_handle].c_str(), log_buffer));
		return 0;
	}
	
	return std::unique_ptr<shader_program>(new shader_program(program, vertexShader, fragmentShader));
}

shader_program::Cache & shader_program::get_cache()
{
	static Cache s_cache(&shader_program::load);
	return s_cache;
}

void shader_program::bind(const shader_program & program)
{
	glUseProgram(program.m_handle);
}

void shader_program::unbind()
{
	glUseProgram(0);
}

shader_program::shader_program(const GLuint handle, std::shared_ptr<shader> vertexShader, std::shared_ptr<shader> fragmentShader) :
	m_handle(handle),
	m_vertexShader(std::move(vertexShader)),
	m_fragmentShader(std::move(fragmentShader))
{
}

shader_program::~shader_program()
{
	glDetachShader(m_handle, m_vertexShader->m_handle);
	glDetachShader(m_handle, m_fragmentShader->m_handle);
	glDeleteProgram(m_handle);
}

std::string shader_program::get_name() const
{
	return string_utilities::left(m_vertexShader->get_filename(), '.');
}

int shader_program::getAttributeLocation(const char * name)
{
	return glGetAttribLocation(m_handle, name);
}

int shader_program::getUniformLocation(const char * name)
{
	return glGetUniformLocation(m_handle, name);
}
