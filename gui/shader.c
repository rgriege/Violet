#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "violet/utility/log.h"

#include "violet/gui/shader.h"

b8 vlt_shader_init(vlt_shader * shader, const char * filename, shader_type type)
{
	b8 retval = false;
	FILE * file = fopen(filename, "r");
	if (!file)
	{
		log_write("Could not open shader file '%s'", filename);
		return retval;
	}

	fseek(file, 0, SEEK_END);
	const size_t size = ftell(file);
	rewind(file);
	char * file_buf = malloc(size + 1);
	if (fread(file_buf, 1, size, file) != size)
	{
		log_write("Failed to read shader file '%s'", filename);
		goto err;
	}
	file_buf[size] = 0;

	GLuint const shader_handle = glCreateShader(type == VERTEX_SHADER ?
		GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(shader_handle, 1, (const GLchar **)&file_buf, 0);

	glCompileShader(shader_handle);
	GLint compiled;
	glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint log_len;
		glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_len);
		char * log_buf = malloc(log_len);
		glGetShaderInfoLog(shader_handle, log_len, NULL, log_buf);
		log_write("Compilation error in shader file '%s': %s",
			filename, log_buf);
		free(log_buf);
		goto err;
	}

	shader->handle = shader_handle;
	retval = true;
err:
	free(file_buf);
	fclose(file);
	return retval;
}

void vlt_shader_destroy(vlt_shader * shader)
{
	glDeleteShader(shader->handle);
	shader->handle = 0;
}

static b8 _shader_program_load_internal(vlt_shader_program * p,
                                        const char * vertex_shader_filename,
                                        const char * frag_shader_filename)
{
	b8 retval = false;
	if (!vlt_shader_init(&p->vertex_shader, vertex_shader_filename, VERTEX_SHADER))
		goto out;

	if (!vlt_shader_init(&p->frag_shader, frag_shader_filename, FRAGMENT_SHADER))
		goto err_frag;
	
	if (vlt_shader_program_create(p, p->vertex_shader, p->frag_shader))
	{
		retval = true;
		goto out;
	}

	vlt_shader_destroy(&p->frag_shader);
err_frag:
	vlt_shader_destroy(&p->vertex_shader);
out:
	return retval;
}

b8 vlt_shader_program_load(vlt_shader_program * p, const char * name)
{
	const u32 name_len = strlen(name);

	char * vertex_shader_name = malloc(name_len + 6);
	strcpy(vertex_shader_name, name);
	strcpy(vertex_shader_name + name_len, ".vert");

	char * frag_shader_name = malloc(name_len + 6);
	strcpy(frag_shader_name, name);
	strcpy(frag_shader_name + name_len, ".frag");

	const b8 retval = _shader_program_load_internal(p,
		vertex_shader_name, frag_shader_name);

	free(vertex_shader_name);
	free(frag_shader_name);

	return retval;
}

b8 vlt_shader_program_create(vlt_shader_program * p, 
                             vlt_shader vertex_shader,
                             vlt_shader frag_shader)
{
	p->handle = glCreateProgram();

	glAttachShader(p->handle, vertex_shader.handle);
	p->vertex_shader = vertex_shader;
	glAttachShader(p->handle, frag_shader.handle);
	p->frag_shader = frag_shader;

	glLinkProgram(p->handle);
	GLint linked;
	glGetProgramiv(p->handle, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(p->handle, GL_INFO_LOG_LENGTH, &length);
		char * log_buf = malloc(length);
		glGetProgramInfoLog(p->handle, length, NULL, log_buf);
		log_write("Shader link error using '%s' & '%s': %s",
			vertex_shader.filename, frag_shader.filename, log_buf);
		free(log_buf);
		return false;
	}

	return true;
}

void vlt_shader_program_bind(const vlt_shader_program * p)
{
	glUseProgram(p->handle);
}

void vlt_shader_program_unbind()
{
	glUseProgram(0);
}

void vlt_shader_program_destroy(vlt_shader_program * p)
{
	glDetachShader(p->handle, p->vertex_shader.handle);
	vlt_shader_destroy(&p->vertex_shader);
	glDetachShader(p->handle, p->frag_shader.handle);
	vlt_shader_destroy(&p->frag_shader);
	glDeleteProgram(p->handle);
	p->handle = 0;
}

s32 vlt_shader_program_attrib(const vlt_shader_program * p, const char * name)
{
	return glGetAttribLocation(p->handle, name);
}

s32 vlt_shader_program_uniform(const vlt_shader_program * p, const char * name)
{
	return glGetUniformLocation(p->handle, name);
}

