// ============================================================================

#include <fstream>
#include <GL/glew.h>
#include <map>
#include <string>

#include "violet/core/defines.h"
#include "violet/graphics/shader/shader.h"
#include "violet/log/log.h"
#include "violet/utility/buffer.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/string_utilities.h"

using namespace vlt;

// ============================================================================

std::map<GLuint, std::string> g_shader_filenames;

// ============================================================================

std::unique_ptr<Shader> Shader::create(const char * filename, Type type)
{
	std::ifstream stream(filename, std::ios_base::binary);
	if (!stream)
	{
		log(Formatted_String<128>().sprintf("Could not open Shader file '%s'", filename));
		return nullptr;
	}

	std::streampos const start = stream.tellg();
	stream.seekg(0, std::ios::end);
	u32 const size = static_cast<u32>(stream.tellg() - start);
	stream.seekg(0, std::ios::beg);
	Heap_Buffer<char> buffer(size + 1);
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
		Heap_Buffer<char> log_buffer(length);
		glGetShaderInfoLog(shader_handle, length, nullptr, log_buffer);
		log(Formatted_String<1024>().sprintf("Compilation error in Shader file '%s': %s", filename, log_buffer.data));
		return nullptr;
	}

	g_shader_filenames[shader_handle] = filename;
	return std::make_unique<Shader>(filename, shader_handle);
}

// ============================================================================

Shader::Shader(const char * _filename, const GLuint _handle) :
	filename(_filename),
	handle(_handle)
{
}

// ----------------------------------------------------------------------------

Shader::~Shader()
{
	glDeleteShader(handle);
}

// ============================================================================

std::unique_ptr<Shader_Program> Shader_Program::load(const char * name)
{
	std::string nameStr = name;
	return load((nameStr + ".vert").c_str(), (nameStr + ".frag").c_str());
}

// ----------------------------------------------------------------------------

std::unique_ptr<Shader_Program> Shader_Program::load(const char * vertex_shader_filename, const char * fragment_shader_filename)
{
	auto vertex_shader = Shader::create(vertex_shader_filename, Shader::Vertex);
	auto fragment_shader = Shader::create(fragment_shader_filename, Shader::Fragment);
	
	return (vertex_shader != nullptr && fragment_shader != nullptr) ? Shader_Program::create(std::shared_ptr<Shader>(vertex_shader.release()), std::shared_ptr<Shader>(fragment_shader.release())) : nullptr;
}

// ----------------------------------------------------------------------------

std::unique_ptr<Shader_Program> Shader_Program::create(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader)
{
	GLuint program = glCreateProgram();

	glAttachShader(program, vertex_shader->handle);
	glAttachShader(program, fragment_shader->handle);

	glLinkProgram(program);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		Heap_Buffer<char> log_buffer(length);
		glGetProgramInfoLog(program, length, nullptr, log_buffer);
		log(Formatted_String<1024>().sprintf("Link error in Shader program using '%s' and '%s': %s", g_shader_filenames[vertex_shader->handle].c_str(), g_shader_filenames[fragment_shader->handle].c_str(), log_buffer));
		return 0;
	}
	
	return std::make_unique<Shader_Program>(program, vertex_shader, fragment_shader);
}

// ----------------------------------------------------------------------------

Shader_Program::Cache & Shader_Program::get_cache()
{
	static Cache s_cache(&Shader_Program::load);
	return s_cache;
}

// ----------------------------------------------------------------------------

void Shader_Program::bind(const Shader_Program & program)
{
	glUseProgram(program.handle);
}

// ----------------------------------------------------------------------------

void Shader_Program::unbind()
{
	glUseProgram(0);
}

// ============================================================================

Shader_Program::Shader_Program(const GLuint _handle, std::shared_ptr<Shader> _vertex_shader, std::shared_ptr<Shader> _fragment_shader) :
	handle(_handle),
	vertex_shader(std::move(_vertex_shader)),
	fragment_shader(std::move(_fragment_shader))
{
}

// ----------------------------------------------------------------------------

Shader_Program::~Shader_Program()
{
	glDetachShader(handle, vertex_shader->handle);
	glDetachShader(handle, fragment_shader->handle);
	glDeleteProgram(handle);
}

// ----------------------------------------------------------------------------

std::string Shader_Program::get_name() const
{
	return String_Utilities::left(vertex_shader->filename, '.');
}

// ----------------------------------------------------------------------------

int Shader_Program::get_attrib_loc(const char * name)
{
	return glGetAttribLocation(handle, name);
}

// ----------------------------------------------------------------------------

int Shader_Program::get_uniform_loc(const char * name)
{
	return glGetUniformLocation(handle, name);
}

// ============================================================================

