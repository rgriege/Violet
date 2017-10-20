#ifndef VIOLET_GRAPHICS_H
#define VIOLET_GRAPHICS_H

#include <GL/glew.h>

const char *gl_get_err_str(GLenum err);

#if defined(DEBUG) || defined(CHECK_GL)
#define GL_ERR_CHECK(label) \
	do { \
		GLenum err; \
		if ((err = glGetError()) != GL_NO_ERROR) { \
			const char *err_str = gl_get_err_str(err); \
			log_error("%s: %s(%x) @ %s:%d", label, err_str, err, \
			          __FILE__, __LINE__); \
		} \
	} while (0)
#define GL_CHECK(func, ...) \
	do { \
		func(__VA_ARGS__); \
		GL_ERR_CHECK(#func); \
	} while (0)
#else
#define GL_ERR_CHECK(label) NOOP
#define GL_CHECK(func, ...) func(__VA_ARGS__)
#endif

#endif // VIOLET_GRAPHICS_H


/* Implementation */

#ifdef GRAPHICS_IMPLEMENTATION

const char *gl_get_err_str(GLenum err)
{
	switch (err) {
	case GL_INVALID_ENUM:
		return "INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "INVALID_OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "OUT_OF_MEMORY";
	case GL_STACK_UNDERFLOW:
		return "STACK_UNDERFLOW";
	case GL_STACK_OVERFLOW:
		return "STACK_OVERFLOW";
	default:
		return "UNKNOWN ERROR";
	}
}

#undef GRAPHICS_IMPLEMENTATION
#endif // GRAPHICS_IMPLEMENTATION
