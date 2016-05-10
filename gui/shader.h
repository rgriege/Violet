#ifndef VIOLET_SHADER_H
#define VIOLET_SHADER_H

#include "violet/core/types.h"

typedef enum shader_type
{
	VERTEX_SHADER,
	FRAGMENT_SHADER
} shader_type;

typedef struct vlt_shader
{
	const char * filename;
	u32 handle;
} vlt_shader;


b8 vlt_shader_init(vlt_shader * shader, const char * filename, shader_type type);
void vlt_shader_destroy(vlt_shader * shader);

typedef struct vlt_shader_program
{
	u32 handle;
	vlt_shader vertex_shader;
	vlt_shader frag_shader;
} vlt_shader_program;

b8 vlt_shader_program_load(vlt_shader_program * prog, const char * name);
b8 vlt_shader_program_create(vlt_shader_program * prog,
                             vlt_shader vertex_shader,
                             vlt_shader frag_shader);
void vlt_shader_program_destroy(vlt_shader_program * p);

void vlt_shader_program_bind(const vlt_shader_program * p);
void vlt_shader_program_unbind();

s32 vlt_shader_program_attrib(const vlt_shader_program * p, const char * name);
s32 vlt_shader_program_uniform(const vlt_shader_program * p, const char * name);

#endif
