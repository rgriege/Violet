#ifndef VIOLET_SDL_GL_H
#define VIOLET_SDL_GL_H

/* GL */

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

/* Mesh */

typedef struct mesh_t
{
	u32 vbo;
	u32 sz;
} mesh_t;

void mesh_init(mesh_t *m, const v2f *poly, u32 n);
void mesh_destroy(mesh_t *m);
void mesh_poly(const mesh_t *m, v2f *poly);
void mesh_bind(const mesh_t *m);
void mesh_unbind(void);
void mesh_set_vertices(mesh_t *m, const v2f *v, u32 n);

/* Texture */

b32  texture_load(gui_texture_t *tex, const char *filename);
void texture_init(gui_texture_t *tex, u32 w, u32 h, u32 fmt, const void *data);
void texture_destroy(gui_texture_t *tex);
void texture_coords_from_poly(mesh_t *tex_coords, const v2f *v, u32 n);
void texture_bind(const gui_texture_t *tex);
void texture_unbind(void);

/* Shader */

typedef enum shader_type
{
	VERTEX_SHADER,
	FRAGMENT_SHADER
} shader_type_e;

typedef struct shader_t
{
	u32 handle;
} shader_t;


b32  shader_init_from_string(shader_t *shader, const char *str,
                             shader_type_e type, const char *name);
b32  shader_init_from_file(shader_t *shader, const char *fname,
                           shader_type_e type, const char *name);
void shader_destroy(shader_t *shader);

typedef struct shader_prog_t
{
	u32 handle;
	shader_t vertex_shader;
	shader_t frag_shader;
} shader_prog_t;

b32  shader_program_load_from_files(shader_prog_t *prog,
                                    const char *vert_fname,
                                    const char *frag_fname,
                                    const char *name);
b32  shader_program_load_from_strings(shader_prog_t *prog,
                                      const char *vert_str,
                                      const char *frag_str,
                                      const char *name);
b32  shader_program_create(shader_prog_t *prog, shader_t vertex_shader,
                           shader_t frag_shader, const char *name);
void shader_program_destroy(shader_prog_t *p);
void shader_program_bind(const shader_prog_t *p);
void shader_program_unbind(void);
s32  shader_program_attrib(const shader_prog_t *p, const char *name);
s32  shader_program_uniform(const shader_prog_t *p, const char *name);

/* Image */

b32  img_load(gui_img_t *img, const char *filename);
void img_init(gui_img_t *img, u32 w, u32 h, u32 fmt, void *data);
void img_destroy(gui_img_t *img);

/* Font */

#ifndef GUI_FONT_FILE_PATH
#define GUI_FONT_FILE_PATH "Roboto.ttf"
#endif

typedef struct font_t
{
	const char *filename;
	s32 size;
	s32 num_glyphs;
	gui_font_metrics_t metrics;
	void *char_info;
	void *index_map;
	gui_texture_t texture;
} font_t;

b32  font_load(font_t *f, const char *filename, s32 size);
void font_destroy(font_t *f);

typedef enum window_flags
{
	WINDOW_BORDERLESS = 1 << 0,
	WINDOW_RESIZABLE  = 1 << 1,
	WINDOW_MAXIMIZED  = 1 << 2,
	WINDOW_FULLSCREEN = 1 << 3,
	WINDOW_CENTERED   = 1 << 4,
	/* vsync protip:
	 * enable this and call window_end_frame_ex with 0 target_frame_milli to
	 * avoid sleeping between draws, this seems to block on glClear in
	 * window_begin_frame to rate limit according to the system. */
	WINDOW_NOVSYNC    = 1 << 5,
} window_flags_e;

typedef struct window window_t;

window_t *window_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                        window_flags_e flags);
window_t *window_create_ex(s32 x, s32 y, s32 w, s32 h, const char *title,
                           window_flags_e flags, const char *font_file_path);
void      window_destroy(window_t *window);

b32    window_begin_frame(window_t *window);
void   window_end_frame(window_t *window);
void   window_end_frame_ex(window_t *window, u32 target_frame_milli,
                           u32 idle_frame_milli, u32 idle_start_milli);
void   window_move(const window_t *window, s32 dx, s32 dy);
b32    window_is_maximized(const window_t *window);
void   window_minimize(window_t *window);
void   window_maximize(window_t *window);
void   window_restore(window_t *window);
void   window_fullscreen(window_t *window);
void   window_set_title(window_t *window, const char *title);
void   window_run(window_t *window, u32 fps, b32(*ufunc)(window_t*, void*), void *udata);

gui_t *window_get_gui(window_t *window);
const gui_img_t *window_get_img(window_t *window, const char *fname);

void   window_drag(window_t *window, s32 x, s32 y, s32 w, s32 h);

s32    window_get_scale_for_dpi(const window_t *window);
void   mouse_pos_global(const window_t *window, s32 *x, s32 *y);

#endif // VIOLET_SDL_GL_H

#ifdef SDL_GL_IMPLEMENTATION

#define SDL_MAIN_HANDLED
#include <SDL.h>
#define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_STATIC
#include "stbi.h"
#define STB_RECT_PACK_IMPLEMENTATION
// #define STBRP_STATIC
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
// #define STBTT_STATIC
#define stbtt_uint8 u8
#define stbtt_int8 s8
#define stbtt_uint16 u16
#define stbtt_int16 s16
#define stbtt_uint32 u32
#define stbtt_int32 s32
#define STBTT_malloc(x, u) ((allocator_t*)(u))->malloc_(x, u  MEMCALL_LOCATION)
#define STBTT_free(x, u)   ((allocator_t*)(u))->free_(x, u  MEMCALL_LOCATION)
#include "stb_truetype.h"

static const char *g_vertex_shader;
static const char *g_fragment_shader;

/* GL */

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

/* Mesh */

void mesh_init(mesh_t *m, const v2f *poly, u32 n)
{
	GL_CHECK(glGenBuffers, 1, &m->vbo);
	mesh_set_vertices(m, poly, n);
}

void mesh_destroy(mesh_t *m)
{
	if (m->vbo != 0)
		GL_CHECK(glDeleteBuffers, 1, &m->vbo);
}

void mesh_poly(const mesh_t *m, v2f *poly)
{
	array_clear(poly);
	array_reserve(poly, m->sz);
	array_sz(poly) = m->sz;
	mesh_bind(m);
	GL_CHECK(glGetBufferSubData, GL_ARRAY_BUFFER, 0, m->sz * 2 * sizeof(GL_FLOAT), poly);
}

void mesh_bind(const mesh_t *m)
{
	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, m->vbo);
}

void mesh_unbind(void)
{
	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

void mesh_set_vertices(mesh_t *m, const v2f *v, u32 n)
{
	mesh_bind(m);
	m->sz = n;
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, m->sz * 2 * sizeof(GL_FLOAT), v, GL_STATIC_DRAW);
}

/* Texture */

b32 texture_load(gui_texture_t *tex, const char *filename)
{
	b32 ret = false;
	int w, h;
	stbi_set_flip_vertically_on_load(true);
	u8 *image = stbi_load(filename, &w, &h, NULL, 4);
	if (image) {
		texture_init(tex, w, h, GL_RGBA, image);
		stbi_image_free(image);
		ret = true;
	}
	return ret;
}

void texture_init(gui_texture_t *tex, u32 w, u32 h, u32 fmt, const void *data)
{
	tex->width = w;
	tex->height = h;
	tex->blend = GUI_BLEND_NRM;

	GL_CHECK(glGenTextures, 1, &tex->handle);
	texture_bind(tex);

	GL_CHECK(glTexImage2D, GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt,
			GL_UNSIGNED_BYTE, data);

	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void texture_destroy(gui_texture_t *tex)
{
	if (tex->handle != 0) {
		GL_CHECK(glDeleteTextures, 1, &tex->handle);
		tex->handle = 0;
	}
}

void texture_coords_from_poly(mesh_t *tex_coords, const v2f *v, u32 n)
{
	box2f extent;
	v2f dimension;
	v2f *coords;

	polyf_bounding_box(v, n, &extent);
	dimension = v2f_sub(extent.max, extent.min);

	array_init_ex(coords, n, g_temp_allocator);
	for (const v2f *vi = v, *vn = v + n; vi != vn; ++vi)
		array_append(coords, v2f_div(v2f_sub(*vi, extent.min), dimension));
	mesh_init(tex_coords, coords, n);
	array_destroy(coords);
}

void texture_bind(const gui_texture_t *tex)
{
	GL_CHECK(glBindTexture, GL_TEXTURE_2D, tex->handle);
}

void texture_unbind(void)
{
	GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);
}


/* Shader */

static const char *g_shader_type_names[2] = {
	[VERTEX_SHADER] = "vertex",
	[FRAGMENT_SHADER] = "fragment",
};

b32 shader_init_from_string(shader_t *shader, const char *str,
                            shader_type_e type, const char *name)
{
	const char *type_name = g_shader_type_names[type];
	b32 retval = false;
	GLint compiled = GL_FALSE;
	GLenum err = GL_NO_ERROR;

	/* NOTE(rgriege): Do OpenGL error checking here even in release builds
	 * since a lot of graphics errors have shown up here, and the cause
	 * is typically very opaque to the user.  Additionally, we can
	 * add more customized log messages, including more than just the OpenGL
	 * function name.  I didn't profile this code with error checking,
	 * but shader compilation is supposedly expensive, so a little
	 * error checking shouldn't impact performance too badly. */

	glGetError(); /* clear error flag */

	shader->handle = glCreateShader(  type == VERTEX_SHADER
	                                ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glCreateShader(%s, %s) = %s", name,
		          type_name, gl_get_err_str(err));
		goto err;
	}

	glShaderSource(shader->handle, 1, (const GLchar **)&str, 0);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glShaderSource(%s, %s) = %s", name,
		          type_name, gl_get_err_str(err));
		goto err;
	}

	glCompileShader(shader->handle);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glCompileShader(%s, %s) = %s", name,
		          type_name, gl_get_err_str(err));
		goto err;
	}

	glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &compiled);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glGetShaderiv(%s, %s, GL_COMPILE_STATUS) = %s", name,
		          type_name, gl_get_err_str(err));
		goto err;
	}

	if (compiled == GL_FALSE) {
		GLint log_len;
		char *log_buf;

		glGetShaderiv(shader->handle, GL_INFO_LOG_LENGTH, &log_len);
		if ((err = glGetError()) != GL_NO_ERROR) {
			log_error("glGetShaderiv(%s, %s, GL_INFO_LOG_LENGTH) = %s", name,
			          type_name, gl_get_err_str(err));
			goto err;
		}

		log_buf = amalloc(log_len, g_temp_allocator);
		glGetShaderInfoLog(shader->handle, log_len, NULL, log_buf);
		if ((err = glGetError()) != GL_NO_ERROR) {
			log_error("glGetShaderInfoLog(%s, %s) = %s", name,
			          type_name, gl_get_err_str(err));
			goto err;
		}

		log_error("Compilation error in %s shader '%s': %s",
		          g_shader_type_names[type], name, log_buf);
		afree(log_buf, g_temp_allocator);
		shader->handle = 0;
		goto err;
	}

	retval = true;
err:
	return retval;
}

b32 shader_init_from_file(shader_t *shader, const char *fname,
                          shader_type_e type, const char *name)
{
	b32 retval = false;
	FILE *file;
	char *file_buf;
	size_t fsize;

	file = file_open(fname, "rb");
	if (!file) {
		log_error("Failed to open file '%s' for %s shader '%s'",
		          fname, g_shader_type_names[type], name);
		return retval;
	}

	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	rewind(file);
	file_buf = amalloc(fsize + 1, g_allocator);
	if (fread(file_buf, 1, fsize, file) != fsize) {
		log_error("Failed to read file '%s' for %s shader '%s'",
		          fname, g_shader_type_names[type], name);
		goto err;
	}
	file_buf[fsize] = 0;

	retval = shader_init_from_string(shader, file_buf, type, fname);

err:
	afree(file_buf, g_allocator);
	fclose(file);
	return retval;
}

void shader_destroy(shader_t *shader)
{
	GL_CHECK(glDeleteShader, shader->handle);
	shader->handle = 0;
}

b32 shader_program_load_from_strings(shader_prog_t *prog,
                                     const char *vert_str,
                                     const char *frag_str,
                                     const char *name)
{
	b32 retval = false;

	if (!shader_init_from_string(&prog->vertex_shader, vert_str,
	                             VERTEX_SHADER, name))
		goto out;

	if (!shader_init_from_string(&prog->frag_shader, frag_str,
	                             FRAGMENT_SHADER, name))
		goto err_frag;

	if (shader_program_create(prog, prog->vertex_shader, prog->frag_shader, name)) {
		retval = true;
		goto out;
	}

	shader_destroy(&prog->frag_shader);
err_frag:
	shader_destroy(&prog->vertex_shader);
out:
	return retval;
}

b32 shader_program_load_from_files(shader_prog_t *prog,
                                   const char *vert_fname,
                                   const char *frag_fname,
                                   const char *name)
{
	b32 retval = false;

	if (!shader_init_from_file(&prog->vertex_shader, vert_fname, VERTEX_SHADER, name))
		goto out;

	if (!shader_init_from_file(&prog->frag_shader, frag_fname, FRAGMENT_SHADER, name))
		goto err_frag;
	
	if (shader_program_create(prog, prog->vertex_shader, prog->frag_shader, name)) {
		retval = true;
		goto out;
	}

	shader_destroy(&prog->frag_shader);
err_frag:
	shader_destroy(&prog->vertex_shader);
out:
	return retval;
}

b32 shader_program_create(shader_prog_t *p, shader_t vertex_shader,
                          shader_t frag_shader, const char *name)
{
	b32 retval = false;
	GLint status = GL_FALSE;
	GLenum err = GL_NO_ERROR;

	/* NOTE(rgriege): Do OpenGL error checking here even in release builds
	 * since a lot of graphics errors have shown up here, and the cause
	 * is typically very opaque to the user.  Additionally, we can
	 * add more customized log messages, including more than just the OpenGL
	 * function name.  I didn't profile this code with error checking,
	 * but shader compilation is supposedly expensive, so a little
	 * error checking shouldn't impact performance too badly. */

	glGetError(); /* clear error flag */

	p->handle = glCreateProgram();
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glCreateProgram(%s) = %s", name, gl_get_err_str(err));
		goto err;
	}

	glAttachShader(p->handle, vertex_shader.handle);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glAttachShader(%s, vertex) = %s", name, gl_get_err_str(err));
		goto err;
	}
	p->vertex_shader = vertex_shader;

	glAttachShader(p->handle, frag_shader.handle);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glAttachShader(%s, fragment) = %s", name, gl_get_err_str(err));
		goto err;
	}
	p->frag_shader = frag_shader;

	glLinkProgram(p->handle);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glLinkProgram(%s) = %s", name, gl_get_err_str(err));
		goto err;
	}

	glGetProgramiv(p->handle, GL_LINK_STATUS, &status);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glGetProgramiv(%s, GL_LINK_STATUS) = %s", name, gl_get_err_str(err));
		goto err;
	}

	if (status == GL_FALSE) {
		GLint length;
		char *log_buf;

		glGetProgramiv(p->handle, GL_INFO_LOG_LENGTH, &length);
		if ((err = glGetError()) != GL_NO_ERROR) {
			log_error("glGetProgramiv(%s, GL_INFO_LOG_LENGTH) = %s", name, gl_get_err_str(err));
			goto err;
		}

		log_buf = amalloc(length, g_temp_allocator);
		glGetProgramInfoLog(p->handle, length, NULL, log_buf);
		if ((err = glGetError()) != GL_NO_ERROR) {
			log_error("glGetProgramInfoLog(%s) = %s", name, gl_get_err_str(err));
			goto err;
		}

		log_error("Link error in shader '%s': %s", name, log_buf);
		afree(log_buf, g_temp_allocator);
		goto err;
	}

#ifdef GUI_VALIDATE_SHADER
	glValidateProgram(p->handle);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glValidateProgram(%s) = %s", name, gl_get_err_str(err));
		goto err;
	}

	glGetProgramiv(p->handle, GL_VALIDATE_STATUS, &status);
	if ((err = glGetError()) != GL_NO_ERROR) {
		log_error("glGetProgramiv(%s, GL_VALIDATE_STATUS) = %s", name, gl_get_err_str(err));
		goto err;
	}

	if (status == GL_FALSE) {
		GLint length;
		char *log_buf;

		glGetProgramiv(p->handle, GL_INFO_LOG_LENGTH, &length);
		if ((err = glGetError()) != GL_NO_ERROR) {
			log_error("glGetProgramiv(%s, GL_INFO_LOG_LENGTH) = %s", name, gl_get_err_str(err));
			goto err;
		}

		log_buf = amalloc(length, g_temp_allocator);
		glGetProgramInfoLog(p->handle, length, NULL, log_buf);
		if ((err = glGetError()) != GL_NO_ERROR) {
			log_error("glGetProgramInfoLog(%s) = %s", name, gl_get_err_str(err));
			goto err;
		}

		log_error("Validation error in shader '%s': %s", name, log_buf);
		afree(log_buf, g_temp_allocator);
		goto err;
	}
#endif

	retval = true;
err:
	return retval;
}

void shader_program_bind(const shader_prog_t *p)
{
	GL_CHECK(glUseProgram, p->handle);
}

void shader_program_unbind(void)
{
	GL_CHECK(glUseProgram, 0);
}

void shader_program_destroy(shader_prog_t *p)
{
	GL_CHECK(glDetachShader, p->handle, p->vertex_shader.handle);
	shader_destroy(&p->vertex_shader);
	GL_CHECK(glDetachShader, p->handle, p->frag_shader.handle);
	shader_destroy(&p->frag_shader);
	GL_CHECK(glDeleteProgram, p->handle);
	p->handle = 0;
}

s32 shader_program_attrib(const shader_prog_t *p, const char *name)
{
	s32 attrib = glGetAttribLocation(p->handle, name);
	GL_ERR_CHECK("glGetAttribLocation");
	return attrib;
}

s32 shader_program_uniform(const shader_prog_t *p, const char *name)
{
	s32 uniform = glGetUniformLocation(p->handle, name);
	GL_ERR_CHECK("glGetUniformLocation");
	return uniform;
}


/* Image */

b32 img_load(gui_img_t *img, const char *filename)
{
	if (!texture_load(img, filename)) {
		log_error("img_load(%s) error", filename);
		return false;
	}
	return true;
}

void img_init(gui_img_t *img, u32 w, u32 h, u32 fmt, void *data)
{
	texture_init(img, w, h, fmt, data);
}

void img_destroy(gui_img_t *img)
{
	texture_destroy(img);
}

/* copied + modified from stbtt_PackFontRangesGatherRects */
static
void vltt_PackFontAllGatherRects(stbtt_pack_context *spc, const stbtt_fontinfo *info,
                                 int font_size, stbrp_rect *rects)
{
	int missing_glyph_added = 0;
	float fh = font_size;
	float scale = fh > 0 ? stbtt_ScaleForPixelHeight(info, fh)
	                     : stbtt_ScaleForMappingEmToPixels(info, -fh);

	for (int glyph = 0; glyph < info->numGlyphs; ++glyph) {
		int x0,y0,x1,y1;
		if (glyph == 0 && (spc->skip_missing || missing_glyph_added)) {
			rects[glyph].w = rects[glyph].h = 0;
		} else {
			stbtt_GetGlyphBitmapBoxSubpixel(info,glyph,
			                                scale * spc->h_oversample,
			                                scale * spc->v_oversample,
			                                0,0,
			                                &x0,&y0,&x1,&y1);
			rects[glyph].w = (stbrp_coord) (x1-x0 + spc->padding + spc->h_oversample-1);
			rects[glyph].h = (stbrp_coord) (y1-y0 + spc->padding + spc->v_oversample-1);
			if (glyph == 0)
				missing_glyph_added = 1;
		}
	}
}

/* copied + modified from stbtt_PackFontRangesRenderIntoRects */
static
int vltt_PackFontAllRenderIntoRects(stbtt_pack_context *spc, const stbtt_fontinfo *info,
                                    int font_size, stbrp_rect *rects,
                                    stbtt_packedchar *chardata, int *num_unpacked)
{
	int missing_glyph = -1, return_value = 1;
	float fh = font_size;
	float scale = fh > 0 ? stbtt_ScaleForPixelHeight(info, fh)
	                     : stbtt_ScaleForMappingEmToPixels(info, -fh);

	*num_unpacked = 0;

	for (int glyph = 0; glyph < info->numGlyphs; ++glyph) {
		float recip_h,recip_v,sub_x,sub_y;
		recip_h = 1.0f / spc->h_oversample;
		recip_v = 1.0f / spc->v_oversample;
		sub_x = stbtt__oversample_shift(spc->h_oversample);
		sub_y = stbtt__oversample_shift(spc->v_oversample);
		stbrp_rect *r = &rects[glyph];
		if (r->was_packed && r->w != 0 && r->h != 0) {
			stbtt_packedchar *bc = &chardata[glyph];
			int advance, lsb, x0,y0,x1,y1;
			stbrp_coord pad = (stbrp_coord) spc->padding;

			// pad on left and top
			r->x += pad;
			r->y += pad;
			r->w -= pad;
			r->h -= pad;
			stbtt_GetGlyphHMetrics(info, glyph, &advance, &lsb);
			stbtt_GetGlyphBitmapBox(info, glyph,
			                        scale * spc->h_oversample,
			                        scale * spc->v_oversample,
			                        &x0,&y0,&x1,&y1);
			stbtt_MakeGlyphBitmapSubpixel(info,
			                              spc->pixels + r->x + r->y*spc->stride_in_bytes,
			                              r->w - spc->h_oversample+1,
			                              r->h - spc->v_oversample+1,
			                              spc->stride_in_bytes,
			                              scale * spc->h_oversample,
			                              scale * spc->v_oversample,
			                              0,0,
			                              glyph);

			if (spc->h_oversample > 1)
				stbtt__h_prefilter(spc->pixels + r->x + r->y*spc->stride_in_bytes,
				                   r->w, r->h, spc->stride_in_bytes,
				                   spc->h_oversample);

			if (spc->v_oversample > 1)
				stbtt__v_prefilter(spc->pixels + r->x + r->y*spc->stride_in_bytes,
				                   r->w, r->h, spc->stride_in_bytes,
				                   spc->v_oversample);

			bc->x0       = (stbtt_int16)  r->x;
			bc->y0       = (stbtt_int16)  r->y;
			bc->x1       = (stbtt_int16) (r->x + r->w);
			bc->y1       = (stbtt_int16) (r->y + r->h);
			bc->xadvance =                scale * advance;
			bc->xoff     =       (float)  x0 * recip_h + sub_x;
			bc->yoff     =       (float)  y0 * recip_v + sub_y;
			bc->xoff2    =                (x0 + r->w) * recip_h + sub_x;
			bc->yoff2    =                (y0 + r->h) * recip_v + sub_y;

			if (glyph == 0)
				missing_glyph = glyph;
		} else if (spc->skip_missing) {
			return_value = 0;
			++*num_unpacked;
		} else if (r->was_packed && r->w == 0 && r->h == 0 && missing_glyph >= 0) {
			chardata[glyph] = chardata[missing_glyph];
		} else {
			return_value = 0; // if any fail, report failure
			++*num_unpacked;
		}
	}

	return return_value;
}

/* Instead of packing individual ranges, pack all the codepoints that are in the file.
 * Since the stbtt interface wasn't designed for this use, I had to copy+paste+refactor
 * a few functions from the library.  A cleaner approach would be to build a list
 * of ranges from the font's character mapping, but I don't understand the TTF file
 * format well enough to do that.
 *
 * This function is copied + modified from stbtt_PackFontRanges */
static
int vltt_PackFontAll(stbtt_pack_context *spc, stbtt_fontinfo *info,
                     int font_size, stbtt_packedchar *chardata, int *num_unpacked)
{
	const int n = info->numGlyphs;
	int return_value = 1;
	stbrp_rect *rects;

	// flag all characters as NOT packed
	for (int i = 0; i < n; ++i)
		chardata[i].x0 = chardata[i].y0 = chardata[i].x1 = chardata[i].y1 = 0;

	rects = STBTT_malloc(sizeof(*rects) * n, spc->user_allocator_context);
	if (!rects)
		return 0;

	vltt_PackFontAllGatherRects(spc, info, font_size, rects);

	stbtt_PackFontRangesPackRects(spc, rects, n);

	return_value = vltt_PackFontAllRenderIntoRects(spc, info, font_size, rects, chardata, num_unpacked);

	STBTT_free(rects, spc->user_allocator_context);
	return return_value;
}

static
int vltt_PackFont(stbtt_fontinfo *info, int font_size, stbtt_packedchar *chardata, gui_texture_t *tex)
{
#ifdef __EMSCRIPTEN__
	const s32 bpp = 2;
#else
	const s32 bpp = 1;
#endif
	temp_memory_mark_t mark = temp_memory_save(g_temp_allocator);
	unsigned char *bitmap = NULL;
	s32 w = 512, h = 512;
	s32 h_oversample = 3;
	stbtt_pack_context context;
	b32 packed = false, failed = false;
	s32 num_unpacked = 0;

	while (!packed && !failed) {
		temp_memory_restore(mark);
		bitmap = amalloc(w * h * bpp, g_temp_allocator);
		/* NOTE(rgriege): otherwise bitmap has noise at the bottom */
		memset(bitmap, 0, w * h * bpp);

		if ((failed = !stbtt_PackBegin(&context, bitmap, w, h, w * bpp, 1, g_temp_allocator)))
			break;

		stbtt_PackSetOversampling(&context, h_oversample, 1);

		if (vltt_PackFontAll(&context, info, font_size, chardata, &num_unpacked)) {
			stbtt_PackEnd(&context); // not really necessary, handled by memory mark
			packed = true;
		} else {
			const s32 num_glyphs_packed = info->numGlyphs - num_unpacked;
			s32 est_glyphs_packed = num_glyphs_packed;
			s32 est_glyphs_packed_prev;
			do {
				est_glyphs_packed_prev = est_glyphs_packed;
				if (w < 2048) {
					w *= 2;
					est_glyphs_packed *= 2;
				} else if (h_oversample > 1) {
					const s32 old_h_oversample = h_oversample;
					h_oversample = 1;
					est_glyphs_packed *= old_h_oversample;
				} else if (h < 2048) {
					h *= 2;
					est_glyphs_packed *= 2;
				}
			} while (   est_glyphs_packed < info->numGlyphs / 2
			         && est_glyphs_packed > est_glyphs_packed_prev);
			// fail on fonts needing too large a texture,
			// but try one last time if we've changed a parameter
			failed = (est_glyphs_packed == num_glyphs_packed);
		}
	}

	if (packed) {
#ifdef __EMSCRIPTEN__
		unsigned char *row = amalloc(w * bpp, g_temp_allocator);
		for (s32 r = 0; r < h; ++r) {
			memset(row, ~0, w * bpp);
			for (s32 c = 0; c < w; ++c)
				row[c * bpp + bpp - 1] = bitmap[r * w * bpp + c];
			memcpy(&bitmap[r * w * bpp], row, w * bpp);
		}
		texture_init(tex, w, h, GL_LUMINANCE_ALPHA, bitmap);
#else
		texture_init(tex, w, h, GL_RED, bitmap);
		GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
		GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
		GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
		GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
#endif
	}

	temp_memory_restore(mark);
	return packed;
}

/* See https://docs.microsoft.com/en-us/typography/opentype/spec/cmap */
static
size_t font__index_map_size(const stbtt_fontinfo *info)
{
	stbtt_uint8 *data = info->data;
	stbtt_uint32 index_map = info->index_map;
	stbtt_uint16 format = ttUSHORT(data + index_map + 0);

	if (format == 0) { // byte encoding table
		return ttUSHORT(data + index_map + 2);
	} else if (format == 2) { // high-byte mapping through table
		return ttUSHORT(data + index_map + 2);
	} else if (format == 4) { // segment mapping to delta values
		return ttUSHORT(data + index_map + 2);
	} else if (format == 6) { // trimmed table mapping
		return ttUSHORT(data + index_map + 2);
	} else if (format == 8) { // mixed 16-bit and 32-bit coverage
		return ttULONG(data + index_map + 4);
	} else if (format == 10) { // trimmed array
		return ttULONG(data + index_map + 4);
	} else if (format == 12) { // segmented coverage
		return ttULONG(data + index_map + 4);
	} else if (format == 13) { // many-to-one range mappings
		return ttULONG(data + index_map + 4);
	} else if (format == 14) { // unicode variation sequences
		return ttULONG(data + index_map + 2);
	} else {
		assert(0);
		return 0;
	}
}

static
void font__index_map_copy(font_t *f, const stbtt_fontinfo *info, allocator_t *alloc)
{
	stbtt_uint8 *data = info->data;
	stbtt_uint32 index_map = info->index_map;
	size_t size = font__index_map_size(info);

	f->index_map = amalloc(size, alloc);
	memcpy(f->index_map, data + index_map, size);
}

static
int font__get_index_for_codepoint(const font_t *f, s32 codepoint)
{
	/* NOTE(rgriege): kinda hacky - only works as long as stbtt_FindGlyphIndex
	 * doesn't use any other fields from info.  Storing our own hash map instead
	 * of the TTF's index map may be faster & more robust. */
	stbtt_fontinfo info = {
		.data = f->index_map,
		.index_map = 0,
	};
	return stbtt_FindGlyphIndex(&info, codepoint);
}


b32 font_load(font_t *f, const char *filename, s32 size)
{
	b32 retval = false;
	stbtt_fontinfo info = { .userdata = g_temp_allocator };
	int ascent, descent, line_gap;
	r32 scale;
	void *ttf;

	f->char_info = NULL;

	if (!(ttf = file_read_all(filename, "rb", NULL, g_temp_allocator)))
		goto out;

	if (!stbtt_InitFont(&info, ttf, stbtt_GetFontOffsetForIndex(ttf, 0)))
		goto err_ttf;

	f->num_glyphs = info.numGlyphs;
	log_debug("packing %d glyphs for %s:%d", f->num_glyphs, filename, size);
	f->char_info = amalloc(f->num_glyphs * sizeof(stbtt_packedchar), g_allocator);

	if (!vltt_PackFont(&info, size, f->char_info, &f->texture))
		goto err_pack;

	font__index_map_copy(f, &info, g_allocator);

	f->filename = filename;
	f->size = size;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
	scale = stbtt_ScaleForPixelHeight(&info, size);
	f->metrics.ascent = scale * ascent;
	f->metrics.descent = scale * descent;
	f->metrics.line_gap = scale * line_gap;
	f->metrics.newline_dist = scale * (ascent - descent + line_gap);
	retval = true;

err_pack:
	if (!retval)
		afree(f->char_info, g_allocator);
err_ttf:
	afree(ttf, g_temp_allocator);
out:
	return retval;
}

void font_destroy(font_t *f)
{
	afree(f->index_map, g_allocator);
	afree(f->char_info, g_allocator);
	texture_destroy(&f->texture);
}

typedef enum gui_vbo_type
{
	VBO_VERT,
	VBO_COLOR,
	VBO_TEX,
	VBO_COUNT
} gui_vbo_type_e;

typedef struct cached_img
{
	gui_img_t img;
	u32 id;
} cached_img_t;

typedef struct window
{
	SDL_Window *window;
	SDL_GLContext gl_context;
	SDL_Window *parent_window;
	SDL_GLContext parent_gl_context;
	/* rendering */
	u32 vao, vbo[VBO_COUNT];
	shader_prog_t shader;
#ifdef SDL_GL_ES_2
	s32 shader_attrib_loc[VBO_COUNT];
#endif
	gui_texture_t texture_white;
	gui_texture_t texture_white_dotted;

	v2i restore_pos;
	v2i restore_dim;
	box2i drag_area;

	/* style */
	SDL_Cursor *cursors[GUI_CURSOR_COUNT];
	char font_file_path[256];
	array(font_t) fonts;
	font_t *last_font;
	s32 last_font_size;
	array(cached_img_t) imgs;

	gui_t *gui;
} window_t;

static u32 g_window_cnt = 0;

void window_move(const window_t *window, s32 dx, s32 dy)
{
	s32 x, y;
	SDL_GetWindowPosition(window->window, &x, &y);
	SDL_SetWindowPosition(window->window, x + dx, y + dy);
}

static
b32 window__get_display_usable_bounds(s32 display_idx, SDL_Rect *rect)
{
	if (SDL_GetDisplayUsableBounds(display_idx, rect) != 0) {
		log_error("SDL_GetDisplayUsableBounds failed: %s", SDL_GetError());
		return false;
	}

#ifdef __APPLE__
	static SDL_Rect usable_bounds = {0};
	if (usable_bounds.w != 0) {
		/* SDL_GetDisplayUsableBounds returns SDL_GetDisplayBounds in later calls...
		 * I don't like caching this, because the usable region could change while
		 * the app is running, but it's the only option on Mac. */
		*rect = usable_bounds;
	} else {
		/* window origin is supposed to be in the top-left corner,
		 * but for some reason SDL_GetDisplayUsableBounds
		 * returns it in the bottom-left corner on Mac */
		SDL_Rect bounds;
		if (SDL_GetDisplayBounds(display_idx, &bounds) != 0) {
			log_error("SDL_GetDisplayBounds failed: %s", SDL_GetError());
			return false;
		}
		rect->y = bounds.h - rect->h - rect->y;
		usable_bounds = *rect;
	}
#endif
	return true;
}

static
b32 window__maximum_window_rect(const window_t *window, SDL_Rect *rect)
{
	int display_idx;

	display_idx = SDL_GetWindowDisplayIndex(window->window);
	if (display_idx < 0) {
		log_error("SDL_GetWindowDisplayIndex failed: %s", SDL_GetError());
		return false;
	}

	if (!window__get_display_usable_bounds(display_idx, rect))
		return false;

	return true;
}

b32 window_is_maximized(const window_t *window)
{
	SDL_Rect rect;
	v2i dim;
	gui_dim(window->gui, &dim.x, &dim.y);
	return window__maximum_window_rect(window, &rect)
	    && dim.x == rect.w
	    && dim.y == rect.h;
}

void window_minimize(window_t *window)
{
	SDL_MinimizeWindow(window->window);
}

static
void window__store_window_rect(window_t *window, v2i pos, v2i dim)
{
	window->restore_pos.x = pos.x;
	window->restore_pos.y = pos.y;
	window->restore_dim.x = dim.x;
	window->restore_dim.y = dim.y;
}

static
void window__store_current_window_rect(window_t *window)
{
	v2i pos, dim;
	SDL_GetWindowPosition(window->window, &pos.x, &pos.y);
	SDL_GetWindowSize(window->window, &dim.x, &dim.y);
	window__store_window_rect(window, pos, dim);
}

void window_maximize(window_t *window)
{
	/* NOTE(rgriege): needs a workaround because
	 * SDL_MaximizeWindow consumes the menu bar on windoge */
	SDL_Rect rect;
	window__store_current_window_rect(window);
	if (window__maximum_window_rect(window, &rect)) {
		SDL_SetWindowPosition(window->window, rect.x, rect.y);
		SDL_SetWindowSize(window->window, rect.w, rect.h);
	} else {
		/* fallback */
		SDL_MaximizeWindow(window->window);
	}
}

static
void window__restore(window_t *window, v2i pos, v2i dim)
{
	/* NOTE(rgriege): order is important here */
	SDL_SetWindowSize(window->window, dim.x, dim.y);
	SDL_SetWindowPosition(window->window, pos.x, pos.y);
}

static const s32 g_window_border_ratio = 10;

void window_restore(window_t *window)
{
	v2i pos, dim;
	SDL_Rect rect;

	if (   window__maximum_window_rect(window, &rect)
	    && (rect.w == window->restore_dim.x && rect.h == window->restore_dim.y)) {
		SDL_GetWindowSize(window->window, &dim.x, &dim.y);
		const v2i border = v2i_scale_inv(dim, g_window_border_ratio);
		dim = v2i_sub(dim, v2i_scale(border, 2));
		pos = border;
	} else {
		pos = window->restore_pos;
		dim = window->restore_dim;
	}

	window__restore(window, pos, dim);
}

void window_fullscreen(window_t *window)
{
	SDL_Rect rect;
	window__store_current_window_rect(window);
	if (window__maximum_window_rect(window, &rect))
		SDL_MaximizeWindow(window->window);
}

void window_set_title(window_t *window, const char *title)
{
	SDL_SetWindowTitle(window->window, title);
}

static
font_t *window__find_font(window_t *window, s32 size)
{
	array_iterate(window->fonts, i, n)
		if (window->fonts[i].size == size)
			return &window->fonts[i];
	return NULL;
}

static
font_t *window__find_smaller_font(window_t *window, s32 size)
{
	font_t *nearest = NULL;
	s32 max_size = 0;
	array_iterate(window->fonts, i, n) {
		if (   window->fonts[i].char_info
		    && window->fonts[i].size < size
		    && window->fonts[i].size > max_size) {
			nearest = &window->fonts[i];
			max_size = window->fonts[i].size;
		}
	}
	return nearest;
}

static
void *window__get_font(void *handle, s32 size)
{
	window_t *window = handle;
	font_t *font;

	if (window->last_font_size == size)
		return window->last_font;

	window->last_font_size = size;

	if ((font = window__find_font(window, size))) {
		window->last_font = font->char_info ? font : window__find_smaller_font(window, size);
		return window->last_font;
	}

	window->last_font = NULL;
	font = array_append_null(window->fonts);
	if (font_load(font, window->font_file_path, size)) {
		window->last_font = font;
		return window->last_font;
	} else {
		/* keep empty entries around so we don't try to load them again */
		memclr(*font);
		font->filename = window->font_file_path;
		font->size = size;
		window->last_font = window__find_smaller_font(window, size);
		return window->last_font;
	}
}

static
b32 window__get_font_metrics(void *handle, gui_font_metrics_t *metrics)
{
	const font_t *font = handle;
	*metrics = font->metrics;
	return true;
}

static
b32 window__get_char_quad(void *handle, s32 codepoint, r32 x, r32 y, gui_char_quad_t *quad)
{
	const font_t *font = handle;
	const r32 x0 = x;
#ifndef NDEBUG
	const r32 y0 = y;
#endif
	const int index = font__get_index_for_codepoint(font, codepoint);
	stbtt_aligned_quad q;

	if (index == 0)
		return false;

	stbtt_GetPackedQuad(font->char_info, font->texture.width, font->texture.height,
	                    index, &x, &y, &q, 0);
	assert(y0 == y);

	/* NOTE(rgriege): stbtt assumes y=0 at top, but for violet y=0 is at bottom */
	quad->texture = font->texture;
	quad->x0 = q.x0;
	quad->y0 = y + (y - q.y1);
	quad->s0 = q.s0;
	quad->t0 = q.t1;
	quad->x1 = q.x1;
	quad->y1 = quad->y0 + (q.y1 - q.y0);
	quad->s1 = q.s1;
	quad->t1 = q.t0;
	quad->advance = x - x0;
	return true;
}

static
cached_img_t *window__find_img(window_t *window, u32 id)
{
	array_foreach(window->imgs, cached_img_t, ci)
		if (ci->id == id)
			return ci;
	return NULL;
}

gui_t *window_get_gui(window_t *window)
{
	return window->gui;
}

const gui_img_t *window_get_img(window_t *window, const char *fname)
{
	const u32 id = hash_compute(fname);
	cached_img_t *cached_img = window__find_img(window, id);
	if (cached_img)
		return &cached_img->img;

	cached_img = array_append_null(window->imgs);
	cached_img->id = id;
	if (img_load(&cached_img->img, fname))
		return &cached_img->img;

	array_pop(window->imgs);
	return NULL;
}

static
b32 window__supports_opengl_version(int major_version_target, int minor_version_target)
{
	/* SDL docs say we should do this after creating the context */
	int major_version = 0;
	int minor_version = 0;

	if (major_version_target < 3) {
		/* OpenGL < 3 doesn't support fetching version information from
		 * glGetIntegerv, so just say we support it & hope for the best.
		 * Checking the result of the string from GL_VERSION doesn't
		 * seem very well-defined, but I didn't look too hard since
		 * OpenGL 2 support isn't that important for us. */
		return true;
	}

	major_version = 0;
	minor_version = 0;
	GL_CHECK(glGetIntegerv, GL_MAJOR_VERSION, &major_version);
	GL_CHECK(glGetIntegerv, GL_MINOR_VERSION, &minor_version);

	log_info("window OpenGL version number: %d.%d", major_version, minor_version);

	return major_version > major_version_target
	    || (   major_version == major_version_target
	        && minor_version >= minor_version_target);
}

#ifdef CHECK_GL_VERBOSE
static
void window__gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                               GLsizei length, const GLchar *message, const void *userp)
{
	log_level_e level = LOG_DEBUG;
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:         level = LOG_ERROR;   break;
	case GL_DEBUG_SEVERITY_MEDIUM:       level = LOG_WARNING; break;
	case GL_DEBUG_SEVERITY_LOW:          level = LOG_DEBUG;   break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: level = LOG_INFO;    break;
	}

	const char *type_string = "unknown";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:               type_string = "error";               break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_string = "deprecated behavior"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_string = "undefined behavior";  break;
	case GL_DEBUG_TYPE_PORTABILITY:         type_string = "portability";         break;
	case GL_DEBUG_TYPE_PERFORMANCE:         type_string = "performance";         break;
	case GL_DEBUG_TYPE_MARKER:              type_string = "marker";              break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          type_string = "push group";          break;
	case GL_DEBUG_TYPE_POP_GROUP:           type_string = "pop group";           break;
	case GL_DEBUG_TYPE_OTHER:               type_string = "other";               break;
	}

	const char *source_string = "unknown";
	switch (source) {
	case GL_DEBUG_SOURCE_API:             source_string = "api";             break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_string = "window system";   break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: source_string = "shader compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     source_string = "third parth";     break;
	case GL_DEBUG_SOURCE_APPLICATION:     source_string = "application";     break;
	case GL_DEBUG_SOURCE_OTHER:           source_string = "other";           break;
	}

	log_level_write(level, "GL CALLBACK: type = %s, message = %s, from = %s",
	                type_string, message, source_string);
}
#endif

#ifdef _WIN32
static
SDL_HitTestResult window__hit_test(SDL_Window *window, const SDL_Point *point, void *userdata)
{
	const box2i *drag_area = userdata;
	v2i drawable_dim;
	v2i window_dim;
	v2i p;

	SDL_GL_GetDrawableSize(window, &drawable_dim.x, &drawable_dim.y);
	SDL_GetWindowSize(window, &window_dim.x, &window_dim.y);

	p.x = point->x * drawable_dim.x / window_dim.x;
	p.y = drawable_dim.y - point->y * drawable_dim.y / window_dim.y;

	return box2i_contains_point(*drag_area, p)
	     ? SDL_HITTEST_DRAGGABLE
	     : SDL_HITTEST_NORMAL;
}
#endif // _WIN32

window_t *window_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                        window_flags_e flags)
{
	return window_create_ex(x, y, w, h, title, flags, GUI_FONT_FILE_PATH);
}

window_t *window_create_ex(s32 x, s32 y, s32 w, s32 h, const char *title,
                           window_flags_e flags, const char *font_file_path)
{
	window_t *window = acalloc(1, sizeof(window_t), g_allocator);
	if (g_window_cnt == 0) {
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			log_error("SDL_Init(VIDEO) failed: %s", SDL_GetError());
			goto err_sdl;
		}
		window->parent_window = NULL;
	} else {
		window->parent_window = SDL_GL_GetCurrentWindow();
		window->parent_gl_context = SDL_GL_GetCurrentContext();
	}

	/* not checking return values because we check the important ones later */
#ifndef __EMSCRIPTEN__
	// Use OpenGL 3.3 core
	/* not checking return values because we check the important ones later */
	const int gl_major_version_target = 3;
	const int gl_minor_version_target = 3;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version_target);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version_target);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#elif defined(SDL_GL_ES_2)
	// Use OpenGLES 2.0
	const int gl_major_version_target = 2;
	const int gl_minor_version_target = 0;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version_target);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version_target);
#else
	// Use OpenGLES 3.0
	const int gl_major_version_target = 3;
	const int gl_minor_version_target = 0;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version_target);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version_target);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif
	// This is enabled by default.
	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef CHECK_GL_VERBOSE
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	if (SDL_GetNumVideoDisplays() < 1) {
		log_error("could not create window: no video displays found");
		goto err_win;
	}

	SDL_Rect usable_bounds;
	if (!window__get_display_usable_bounds(0, &usable_bounds))
		goto err_ctx;

	/* ensure we don't end up with a 0-width or 0-height window */
	if (w == 0) {
		const s32 border = usable_bounds.w / g_window_border_ratio;
		w = usable_bounds.w - 2 * border;
		x = usable_bounds.x + border;
	}
	if (h == 0) {
		const s32 border = usable_bounds.h / g_window_border_ratio;
		h = usable_bounds.h - 2 * border;
		y = usable_bounds.y + border;
	}

	u32 sdl_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
	if (flags & WINDOW_BORDERLESS)
		sdl_flags |= SDL_WINDOW_BORDERLESS;
	if (flags & WINDOW_RESIZABLE)
		sdl_flags |= SDL_WINDOW_RESIZABLE;
	if (flags & WINDOW_MAXIMIZED) {
		x = usable_bounds.x;
		y = usable_bounds.y;
		w = usable_bounds.w;
		h = usable_bounds.h;
	} else if (flags & WINDOW_CENTERED) {
		w = min(w, usable_bounds.w);
		h = min(h, usable_bounds.h);
		x = (usable_bounds.w - w) / 2 + usable_bounds.x;
		y = (usable_bounds.h - h) / 2 + usable_bounds.y;
	} else {
		x = max(x, usable_bounds.x);
		y = max(y, usable_bounds.y);
		w = min(w, usable_bounds.w - x);
		h = min(h, usable_bounds.h - y);
	}
	if (flags & WINDOW_FULLSCREEN)
		sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	window->window = SDL_CreateWindow(title, x, y, w, h, sdl_flags);
	if (window->window == NULL) {
		log_error("SDL_CreateWindow failed: %s", SDL_GetError());
		goto err_win;
	}

	v2i window_dim;
	SDL_GetWindowSize(window->window, &window_dim.x, &window_dim.y);
	if (window_dim.x == 0 || window_dim.y == 0) {
		/* NOTE(rgriege): I've seen this happen on some browsers w/ Emscripten builds
		 * even though the requested dimensions were valid. */
		v2i window_pos;
		SDL_GetWindowPosition(window->window, &window_pos.x, &window_pos.y);
		log_error("SDL_CreateWindow: invalid window dimensions [%d %d %d %d] -> [%d %d %d %d]",
		          x, y, w, h, window_pos.x, window_pos.y, window_dim.x, window_dim.y);
		goto err_ctx;
	}

	window->gl_context = SDL_GL_CreateContext(window->window);
	if (window->gl_context == NULL) {
		log_error("SDL_CreateContext failed: %s", SDL_GetError());
		goto err_ctx;
	}

#ifndef __EMSCRIPTEN__
	int vsync = flags & WINDOW_NOVSYNC ? 0 : 1;
	if (SDL_GL_SetSwapInterval(vsync) != 0)
		log_warn("SDL_GL_SetSwapInterval(%i) failed: %s", vsync, SDL_GetError());
#endif

	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		log_error("glewInit error: %s", glewGetErrorString(glew_err));
		goto err_glew;
	}

	glGetError(); /* clear error flag */

#ifdef CHECK_GL_VERBOSE
	GL_CHECK(glEnable, GL_DEBUG_OUTPUT);
	GL_CHECK(glDebugMessageCallback, window__gl_debug_callback, NULL);
#endif

	const unsigned char *gl_str = glGetString(GL_VERSION);
	const unsigned char gl_str_unknown[] = "unknown";
	GL_ERR_CHECK("glGetString");
	log_info("OpenGL version: %s", gl_str ? gl_str : gl_str_unknown);
	gl_str = glGetString(GL_RENDERER);
	GL_ERR_CHECK("glGetString");
	log_info("OpenGL renderer: %s", gl_str ? gl_str : gl_str_unknown);
	gl_str = glGetString(GL_VENDOR);
	GL_ERR_CHECK("glGetString");
	log_info("OpenGL vendor: %s", gl_str ? gl_str : gl_str_unknown);

	if (!window__supports_opengl_version(gl_major_version_target, gl_minor_version_target)) {
		log_error("OpenGL context version too small");
		goto err_ver;
	}

#ifndef __EMSCRIPTEN__
	GL_CHECK(glEnable, GL_MULTISAMPLE);
#endif
	GL_CHECK(glEnable, GL_BLEND);
	GL_CHECK(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GL_CHECK(glEnable, GL_SCISSOR_TEST);

	GL_CHECK(glGenVertexArrays, 1, &window->vao);
	GL_CHECK(glGenBuffers, VBO_COUNT, window->vbo);

	static const color_t texture_white_data[1] = { gi_white };
	texture_init(&window->texture_white, 1, 1, GL_RGBA, texture_white_data);

	static const u32 texture_white_dotted_data[] = { 0x00ffffff, 0xffffffff };
	texture_init(&window->texture_white_dotted, 2, 1, GL_RGBA, texture_white_dotted_data);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (!shader_program_load_from_strings(&window->shader, g_vertex_shader,
	                                      g_fragment_shader, "gui ubershader"))
		goto err_white;

#ifdef SDL_GL_ES_2
	window->shader_attrib_loc[VBO_VERT]  = shader_program_attrib(&window->shader, "position");
	window->shader_attrib_loc[VBO_COLOR] = shader_program_attrib(&window->shader, "color");
	window->shader_attrib_loc[VBO_TEX]   = shader_program_attrib(&window->shader, "tex_coord");
#endif

	window->cursors[GUI_CURSOR_DEFAULT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	window->cursors[GUI_CURSOR_RESIZE_NS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	window->cursors[GUI_CURSOR_RESIZE_EW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	window->cursors[GUI_CURSOR_TEXT_INPUT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
#ifdef GUI_USE_CURSOR_BUTTON
	window->cursors[GUI_CURSOR_BUTTON] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
#else
	window->cursors[GUI_CURSOR_BUTTON] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
#endif
	for (u32 i = 0; i < GUI_CURSOR_COUNT; ++i)
		if (!window->cursors[i])
			goto err_cursor;
	strncpy(window->font_file_path, font_file_path, sizeof(window->font_file_path)-1);
	window->fonts = array_create();
	window->last_font = NULL;
	window->last_font_size = 0;
	window->imgs = array_create();

	{
		SDL_Event evt;
		while (SDL_PollEvent(&evt) == 1); /* must be run before SDL_GL_GetDrawableSize */
	}

	v2i dim;
	SDL_GL_GetDrawableSize(window->window, &dim.x, &dim.y);

	window__store_current_window_rect(window);

	box2i_from_xywh(&window->drag_area, 0, 0, 0, 0);
#ifdef _WIN32
	if (flags & WINDOW_BORDERLESS)
		SDL_SetWindowHitTest(window->window, window__hit_test, &window->drag_area);
#endif

	++g_window_cnt;

	gui_fonts_t fonts = {
		.handle = window,
		.get_font = window__get_font,
		.get_metrics = window__get_font_metrics,
		.get_char_quad = window__get_char_quad,
	};

	window->gui = gui_create(dim.x, dim.y, window->texture_white.handle, window->texture_white_dotted.handle, fonts);

	gui_set_window(window->gui, window);

	goto out;

err_cursor:
	for (u32 i = 0; i < GUI_CURSOR_COUNT; ++i)
		if (window->cursors[i])
			SDL_FreeCursor(window->cursors[i]);
err_white:
	texture_destroy(&window->texture_white);
	texture_destroy(&window->texture_white_dotted);
	GL_CHECK(glDeleteBuffers, 3, window->vbo);
	GL_CHECK(glDeleteVertexArrays, 1, &window->vao);
err_ver:
err_glew:
	SDL_GL_DeleteContext(window->gl_context);
	if (window->parent_window)
		SDL_GL_MakeCurrent(window->parent_window, window->parent_gl_context);
err_ctx:
	SDL_DestroyWindow(window->window);
err_win:
	SDL_Quit();
err_sdl:
	afree(window, g_allocator);
	window = NULL;
out:
	return window;
}

void window_destroy(window_t *window)
{
	for (u32 i = 0; i < GUI_CURSOR_COUNT; ++i)
		SDL_FreeCursor(window->cursors[i]);
	array_foreach(window->fonts, font_t, f)
		font_destroy(f);
	array_destroy(window->fonts);
	array_foreach(window->imgs, cached_img_t, ci)
		img_destroy(&ci->img);
	array_destroy(window->imgs);
	shader_program_destroy(&window->shader);
	texture_destroy(&window->texture_white);
	texture_destroy(&window->texture_white_dotted);
	GL_CHECK(glDeleteBuffers, 3, window->vbo);
	GL_CHECK(glDeleteVertexArrays, 1, &window->vao);
	SDL_GL_DeleteContext(window->gl_context);
	SDL_DestroyWindow(window->window);
	if (--g_window_cnt == 0)
		SDL_Quit();
	else if (window->parent_window)
		SDL_GL_MakeCurrent(window->parent_window, window->parent_gl_context);
	gui_destroy(window->gui);
	afree(window, g_allocator);
}

static
void window__drag_cb(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                     s32 offset_x, s32 offset_y, void *udata) {}

void window_drag(window_t *window, s32 x, s32 y, s32 w, s32 h)
{
#ifdef _WIN32
	box2i_from_xywh(&window->drag_area, x, y, w, h);
#else
	gui_t *gui = window->gui;
	gui_style_push(gui, drag, g_gui_style_invis.drag);
	if (gui_drag_rectf(gui, &x, &y, w, h, MB_LEFT, window__drag_cb, NULL)) {
		if (window_is_maximized(window)) {
			/* Shrink the window so that it can be dragged around.
			 * This is going to cause the drag to end, since the drag id won't be the same
			 * next frame.  Fixing this is hard, since the reported mouse position will be
			 * very different next frame even though it hasn't moved. */
			const v2i border = v2i_scale_inv(gui->window_dim, g_window_border_ratio);
			const v2i dim = v2i_sub(gui->window_dim, v2i_scale(border, 2));
			const v2i pos = { .x = border.x, .y = 0 };
			window__restore(window, pos, dim);
		} else if (mouse_pos_changed(gui)) {
			v2i delta;
			mouse_pos_delta(gui, &delta.x, &delta.y);
			window_move(window, delta.x, -delta.y);
		}
		gui_window_drag(gui);
	} else if (gui_window_dragging(gui)) {
		gui_window_drag_end(gui);
	}
	gui_style_pop(gui);
#endif // !_WIN32
}

s32 window_get_scale_for_dpi(const window_t *window)
{
	v2i drawable_dim, window_dim;
	SDL_GL_GetDrawableSize(window->window, &drawable_dim.x, &drawable_dim.y);
	SDL_GetWindowSize(window->window, &window_dim.x, &window_dim.y);
	return max(100 * drawable_dim.x / window_dim.x, 100 * drawable_dim.y / window_dim.y);
}

void mouse_pos_global(const window_t *window, s32 *x, s32 *y)
{
	SDL_GetGlobalMouseState(x, y);
}

b32 window_begin_frame(window_t *window)
{
	gui_t *gui = window->gui;
	const colorf_t bg_color = color_to_colorf(gui_style_c(gui)->bg_color);
	b32 quit = false;
	SDL_Event evt;
	v2i window_dim;
	v2i drawable_dim;
	v2i mouse_pos;
	u32 mouse_btn;
	s32 mouse_wheel = 0;
	const u8 *keys;
	s32 key_cnt;
	char *clipboard;

	SDL_GL_MakeCurrent(window->window, window->gl_context);

	gui_begin_frame(gui);

	gui_events_begin(gui);
	while (SDL_PollEvent(&evt) == 1) {
		switch (evt.type) {
		case SDL_QUIT:
			quit = true;
		break;
		case SDL_MOUSEWHEEL:
			mouse_wheel = evt.wheel.y;
		break;
		case SDL_WINDOWEVENT:
			switch (evt.window.event) {
			case SDL_WINDOWEVENT_CLOSE:
				quit = true;
			break;
			case SDL_WINDOWEVENT_LEAVE:
				gui_event_add_window_leave(gui);
			break;
			default:
				gui_event_add_update(gui);
			break;
			}
		break;
		/* catch keyboard/mouse state events by sending entire state every frame */
		case SDL_MOUSEMOTION:
			gui_event_add_update(gui);
		break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			/* assuming that a clipboard paste action will always occur
			 * due to some mouse/keyboard input, we can only sent the clipboard data
			 * when these events fire. */
			if (SDL_HasClipboardText() && (clipboard = SDL_GetClipboardText())) {
				gui_event_add_clipboard(gui, clipboard);
				SDL_free(clipboard);
			}
			gui_event_add_update(gui);
		break;
		case SDL_TEXTINPUT:
			gui_event_add_text_input(gui, evt.text.text);
		break;
		case SDL_DROPFILE:
			gui_event_add_drop_file(gui, evt.drop.file);
			SDL_free(evt.drop.file);
		break;
		}
	}

	SDL_GL_GetDrawableSize(window->window, &drawable_dim.x, &drawable_dim.y);
	SDL_GetWindowSize(window->window, &window_dim.x, &window_dim.y);
	gui_event_set_window_dim(gui, drawable_dim.x, drawable_dim.y);

	keys = SDL_GetKeyboardState(&key_cnt);
	gui_event_set_keyboard(gui, keys, key_cnt);

	if (gui_window_dragging(gui)) {
		v2i pos;
		SDL_GetWindowPosition(window->window, &pos.x, &pos.y);
		mouse_btn = SDL_GetGlobalMouseState(&mouse_pos.x, &mouse_pos.y);
		mouse_pos = v2i_sub(mouse_pos, pos);
	} else {
		mouse_btn = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
	}
	if (mouse_wheel != 0)
		mouse_btn |= (mouse_wheel > 0 ? MB_WHEELUP : MB_WHEELDOWN);
#ifdef __APPLE__
	if ((keys[KB_LCTRL] || keys[KB_RCTRL]) && (mouse_btn & MB_LEFT)) {
		mouse_btn &= ~MB_LEFT;
		mouse_btn |= MB_RIGHT;
	}
#endif
	/* Drawable size may be greater than window size on high-DPI monitors, but
	 * the mouse position is always reported in window-size coordinates (with inverted y). */
	mouse_pos.x = mouse_pos.x * drawable_dim.x / window_dim.x;
	mouse_pos.y = drawable_dim.y - mouse_pos.y * drawable_dim.y / window_dim.y;
	gui_event_set_mouse_pos(gui, mouse_pos.x, mouse_pos.y);
	gui_event_set_mouse_btn(gui, mouse_btn);

	gui_events_end(gui);

	GL_CHECK(glViewport, 0, 0, drawable_dim.x, drawable_dim.y);

	/* NOTE(rgriege): reset the scissor for glClear */
	GL_CHECK(glScissor, 0, 0, drawable_dim.x, drawable_dim.y);

	GL_CHECK(glClearColor, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	GL_CHECK(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return !quit;
}

static const GLenum g_draw_call_types[GUI_DRAW_COUNT] = {
	GL_POINTS,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_LINES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_TRIANGLES,
	GL_QUAD_STRIP,
	GL_QUADS,
	GL_POLYGON,
};

void window_end_frame(window_t *window)
{
#ifdef SDL_GL_ES_2
	const s32 *loc = window->shader_attrib_loc;
#else
	const s32 loc[VBO_COUNT] = { VBO_VERT, VBO_COLOR, VBO_TEX };
#endif
	gui_t *gui = window->gui;
	gui_render_output_t output;
	GLuint current_texture = 0;
	v2i dim;

	gui_get_render_output(gui, &output);

	u32 current_blend = GUI_BLEND_NRM;

	gui_end_frame(gui);

	gui_dim(gui, &dim.x, &dim.y);

	GL_CHECK(glViewport, 0, 0, dim.x, dim.y);

	GL_CHECK(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GL_CHECK(glDisable, GL_DEPTH_TEST);
	GL_CHECK(glBindVertexArray, window->vao);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, window->vbo[VBO_VERT]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, output.num_verts * sizeof(v2f),
	         output.verts.pos, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, loc[VBO_VERT], 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, loc[VBO_VERT]);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, window->vbo[VBO_COLOR]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, output.num_verts * sizeof(color_t),
	         output.verts.color, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, loc[VBO_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, loc[VBO_COLOR]);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, window->vbo[VBO_TEX]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, output.num_verts * sizeof(v2f),
	         output.verts.tex_coord, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, loc[VBO_TEX], 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, loc[VBO_TEX]);

	GL_CHECK(glUseProgram, window->shader.handle);
	GL_CHECK(glUniform2f, glGetUniformLocation(window->shader.handle, "window_halfdim"),
	         ((r32)dim.x)/2, ((r32)dim.y)/2);

	/* NOTE(rgriege): This method of ordering creates an inconsistency:
	 * panels/layers must be called from top-to-bottom, but widgets/primitives
	 * within a layer must be called from bottom-to-top.  Without introducing a
	 * frame of delay, top-to-bottom panels are unavoidable to ensure that
	 * the top panels receive the input events.  Bottom-to-top widget rendering
	 * is nice for overlaying text on top of a movable widget. Will be a problem
	 * if overlapping widges are in the same panel/layer, but that doesn't seem
	 * like a use case to design for other than dragging icons on a desktop,
	 * which could be 'solved' by placing the dragged icon on a separate layer. */
	for (u32 i = 0; i < output.num_layers; ++i) {
		const gui_layer_t *layer = &output.layers[i];
		GL_CHECK(glScissor, layer->x, layer->y, layer->w, layer->h);
		for (u32 j = 0; j < layer->draw_call_cnt; ++j) {
			const gui_draw_call_t *draw_call = &output.draw_calls[layer->draw_call_idx+j];
			if (draw_call->tex != current_texture) {
				GL_CHECK(glBindTexture, GL_TEXTURE_2D, draw_call->tex);
				current_texture = draw_call->tex;
			}
			if (draw_call->blend != current_blend) {
				switch (draw_call->blend) {
					case GUI_BLEND_NRM:
						GL_CHECK(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						break;
					case GUI_BLEND_ADD:
						GL_CHECK(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
						break;
					case GUI_BLEND_MUL:
						GL_CHECK(glBlendFunc, GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
						break;
				}
				current_blend = draw_call->blend;
			}

			GL_CHECK(glDrawArrays, g_draw_call_types[draw_call->type],
			         draw_call->idx, draw_call->cnt);
		}
	}

	SDL_SetCursor(window->cursors[gui_cursor(gui)]);

	GL_CHECK(glFlush);
	SDL_GL_SwapWindow(window->window);

	if (gui_has_clipboard_text(gui)) {
		char clipboard[GUI_CLIPBOARD_SIZE];
		gui_get_clipboard_text(gui, clipboard);
		if (SDL_SetClipboardText(clipboard) != 0)
			log_error("SDL_SetClipboardText failed: %s", SDL_GetError());
	}

	if (gui_text_input_active(gui) != SDL_IsTextInputActive()) {
		if (gui_text_input_active(gui))
			SDL_StartTextInput();
		else
			SDL_StopTextInput();
	}
}

/* if target_frame_milli is 0, never sleep or complain about long frames,
 * but respect idle values */
void window_end_frame_ex(window_t *window, u32 target_frame_milli,
                         u32 idle_frame_milli, u32 idle_start_milli)
{
	gui_t *gui = window->gui;
	u32 frame_milli;

	window_end_frame(window);
	frame_milli = timepoint_diff_milli(gui_frame_start(gui), timepoint_create());

	if (target_frame_milli && frame_milli > target_frame_milli)
		log_warn("long frame: %ums", frame_milli);
	else if (  timepoint_diff_milli(gui_last_input_time(gui), gui_frame_start(gui))
	         > idle_start_milli)
		SDL_WaitEventTimeout(NULL, idle_frame_milli - frame_milli);
	else if (target_frame_milli)
		time_sleep_milli(target_frame_milli - frame_milli);
}

void window_run(window_t *window, u32 fps, b32(*ufunc)(window_t *window, void *udata), void *udata)
{
	const u32 target_frame_milli = 1000/fps;
	u32 frame_milli;
	b32 quit = false;
	while (window_begin_frame(window) && !quit) {
		vlt_mem_advance_gen();
		quit = ufunc(window, udata);
		window_end_frame(window);
		frame_milli = timepoint_diff_milli(gui_frame_start(window->gui), timepoint_create());
		if (frame_milli < target_frame_milli)
			time_sleep_milli(target_frame_milli - frame_milli);
		else
			log_warn("long frame: %ums", frame_milli);
	}
}

#ifdef __EMSCRIPTEN__
#ifdef SDL_GL_ES_2
static const char *g_vertex_shader =
	"uniform vec2 window_halfdim;\n"
	"attribute vec2 position;\n"
	"attribute vec4 color;\n"
	"attribute vec2 tex_coord;\n"
	"varying vec2 TexCoord;\n"
	"varying vec4 Color;\n"
	"\n"
	"void main() {\n"
	"  vec2 p = (position - window_halfdim) / window_halfdim;\n"
	"  gl_Position = vec4(p.xy, 0.0, 1.0);\n"
	"  TexCoord = tex_coord;\n"
	"  Color = color;\n"
	"}";

static const char *g_fragment_shader =
	"precision mediump float;\n"
	"uniform sampler2D tex;\n"
	"varying vec2 TexCoord;\n"
	"varying vec4 Color;\n"
	"\n"
	"void main() {\n"
	"  gl_FragColor = texture2D(tex, TexCoord) * Color;\n"
	"}";
#else // SDL_GL_ES_2
static const char *g_vertex_shader =
	"#version 300 es\n"
	"layout(location = 0) in vec2 position;\n"
	"layout(location = 1) in vec4 color;\n"
	"layout(location = 2) in vec2 tex_coord;\n"
	"uniform vec2 window_halfdim;\n"
	"out vec2 TexCoord;\n"
	"out vec4 Color;\n"
	"\n"
	"void main() {\n"
	"  vec2 p = (position - window_halfdim) / window_halfdim;\n"
	"  gl_Position = vec4(p.xy, 0.0, 1.0);\n"
	"  TexCoord = tex_coord;\n"
	"  Color = color;\n"
	"}";

static const char *g_fragment_shader =
	"#version 300 es\n"
	"precision mediump float;\n"
	"in vec2 TexCoord;\n"
	"in vec4 Color;\n"
	"uniform sampler2D tex;\n"
	"out vec4 FragColor;\n"
	"\n"
	"void main() {\n"
	"  FragColor = texture(tex, TexCoord) * Color;\n"
	"}";
#endif // SDL_GL_ES_2
#else // __EMSCRIPTEN__
static const char *g_vertex_shader =
	"#version 330\n"
	"layout(location = 0) in vec2 position;\n"
	"layout(location = 1) in vec4 color;\n"
	"layout(location = 2) in vec2 tex_coord;\n"
	"uniform vec2 window_halfdim;\n"
	"out vec2 TexCoord;\n"
	"out vec4 Color;\n"
	"\n"
	"void main() {\n"
	"  vec2 p = (position - window_halfdim) / window_halfdim;\n"
	"  gl_Position = vec4(p.xy, 0.0, 1.0);\n"
	"  TexCoord = tex_coord;\n"
	"  Color = color;\n"
	"}";

static const char *g_fragment_shader =
	"#version 330\n"
	"in vec2 TexCoord;\n"
	"in vec4 Color;\n"
	"uniform sampler2D tex;\n"
	"out vec4 FragColor;\n"
	"\n"
	"void main() {\n"
	"  FragColor = texture(tex, TexCoord) * Color;\n"
	"}";
#endif // __EMSCRIPTEN__

#undef SDL_GL_IMPLEMENTATION
#endif // SDL_GL_IMPLEMENTATION
