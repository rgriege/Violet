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

typedef enum shader_type_t
{
	VERTEX_SHADER,
	FRAGMENT_SHADER
} shader_type_t;

typedef struct shader_t
{
	const char *filename;
	u32 handle;
} shader_t;


b32  shader_init_from_string(shader_t *shader, const char *str,
                             shader_type_t type, const char *id);
b32  shader_init_from_file(shader_t *shader, const char *fname,
                           shader_type_t type);
void shader_destroy(shader_t *shader);

typedef struct shader_prog_t
{
	u32 handle;
	shader_t vertex_shader;
	shader_t frag_shader;
} shader_prog_t;

b32  shader_program_load_from_files(shader_prog_t *prog,
                                    const char *vert_fname,
                                    const char *frag_fname);
b32  shader_program_load_from_strings(shader_prog_t *prog,
                                      const char *vert_str,
                                      const char *frag_str);
b32  shader_program_create(shader_prog_t *prog, shader_t vertex_shader,
                           shader_t frag_shader);
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
	u32 sz;
	s32 num_glyphs;
	gui_font_metrics_t metrics;
	void *char_info;
	gui_texture_t texture;
} font_t;

b32  font_load(font_t *f, const char *filename, u32 sz);
void font_destroy(font_t *f);

typedef enum window_flags_t
{
	WINDOW_BORDERLESS = 1 << 0,
	WINDOW_RESIZABLE  = 1 << 1,
	WINDOW_MAXIMIZED  = 1 << 2,
	WINDOW_FULLSCREEN = 1 << 3,
	WINDOW_CENTERED   = 1 << 4,
} window_flags_t;

typedef struct window window_t;

window_t *window_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                        window_flags_t flags);
window_t *window_create_ex(s32 x, s32 y, s32 w, s32 h, const char *title,
                           window_flags_t flags, const char *font_file_path);
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
void   window_run(window_t *window, u32 fps, b32(*ufunc)(window_t*, void*), void *udata);

gui_t *window_get_gui(window_t *window);
const gui_img_t *window_get_img(window_t *window, const char *fname);

void   window_drag(window_t *window, s32 x, s32 y, s32 w, s32 h);

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

b32 shader_init_from_string(shader_t *shader, const char *str,
                            shader_type_t type, const char *id)
{
	b32 retval = false;
	char *log_buf;
	GLint compiled, log_len;

	shader->handle = glCreateShader(  type == VERTEX_SHADER
	                                ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	GL_ERR_CHECK("glCreateShader");
	GL_CHECK(glShaderSource, shader->handle, 1, (const GLchar **)&str, 0);

	GL_CHECK(glCompileShader, shader->handle);
	GL_CHECK(glGetShaderiv, shader->handle, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GL_CHECK(glGetShaderiv, shader->handle, GL_INFO_LOG_LENGTH, &log_len);
		log_buf = malloc(log_len);
		GL_CHECK(glGetShaderInfoLog, shader->handle, log_len, NULL, log_buf);
		log_error("Compilation error in shader '%s': %s", id, log_buf);
		free(log_buf);
		shader->handle = 0;
		goto err;
	}

	retval = true;
err:
	return retval;
}

b32 shader_init_from_file(shader_t *shader, const char *fname,
                          shader_type_t type)
{
	b32 retval = false;
	FILE *file;
	char *file_buf;
	size_t fsize;

	file = file_open(fname, "rb");
	if (!file) {
		log_error("Could not open shader file '%s'", fname);
		return retval;
	}

	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	rewind(file);
	file_buf = malloc(fsize + 1);
	if (fread(file_buf, 1, fsize, file) != fsize) {
		log_error("Failed to read shader file '%s'", fname);
		goto err;
	}
	file_buf[fsize] = 0;

	retval = shader_init_from_string(shader, file_buf, type, fname);

err:
	free(file_buf);
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
                                     const char *frag_str)
{
	b32 retval = false;

	if (!shader_init_from_string(&prog->vertex_shader, vert_str,
	                             VERTEX_SHADER, "vertex ubershader"))
		goto out;

	if (!shader_init_from_string(&prog->frag_shader, frag_str,
	                             FRAGMENT_SHADER, "fragment ubershader"))
		goto err_frag;

	if (shader_program_create(prog, prog->vertex_shader, prog->frag_shader)) {
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
                                   const char *frag_fname)
{
	b32 retval = false;

	if (!shader_init_from_file(&prog->vertex_shader, vert_fname, VERTEX_SHADER))
		goto out;

	if (!shader_init_from_file(&prog->frag_shader, frag_fname, FRAGMENT_SHADER))
		goto err_frag;
	
	if (shader_program_create(prog, prog->vertex_shader, prog->frag_shader)) {
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
                          shader_t frag_shader)
{
	GLint status, length;
	char *log_buf;

	p->handle = glCreateProgram();
	GL_ERR_CHECK("glCreateProgram");

	GL_CHECK(glAttachShader, p->handle, vertex_shader.handle);
	p->vertex_shader = vertex_shader;
	GL_CHECK(glAttachShader, p->handle, frag_shader.handle);
	p->frag_shader = frag_shader;

	GL_CHECK(glLinkProgram, p->handle);
	GL_CHECK(glGetProgramiv, p->handle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GL_CHECK(glGetProgramiv, p->handle, GL_INFO_LOG_LENGTH, &length);
		log_buf = malloc(length);
		GL_CHECK(glGetProgramInfoLog, p->handle, length, NULL, log_buf);
		log_error("Shader link error using '%s' & '%s': %s",
		          vertex_shader.filename, frag_shader.filename, log_buf);
		free(log_buf);
		return false;
	}

#ifdef GUI_VALIDATE_SHADER
	GL_CHECK(glValidateProgram, p->handle);
	GL_CHECK(glGetProgramiv, p->handle, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
		GL_CHECK(glGetProgramiv, p->handle, GL_INFO_LOG_LENGTH, &length);
		log_buf = malloc(length);
		GL_CHECK(glGetProgramInfoLog, p->handle, length, NULL, log_buf);
		log_error("Shader validation error using '%s' & '%s': %s",
		          vertex_shader.filename, frag_shader.filename, log_buf);
		free(log_buf);
		return false;
	}
#endif

	return true;
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

static
int rgtt_PackFontRanges(stbtt_pack_context *spc, stbtt_fontinfo *info,
                        stbtt_pack_range *ranges, int num_ranges)
{
	int i, j, n, return_value = 1;
	stbrp_rect *rects;

	// flag all characters as NOT packed
	for (i=0; i < num_ranges; ++i)
		for (j=0; j < ranges[i].num_chars; ++j)
			ranges[i].chardata_for_range[j].x0 =
				ranges[i].chardata_for_range[j].y0 =
				ranges[i].chardata_for_range[j].x1 =
				ranges[i].chardata_for_range[j].y1 = 0;

	n = 0;
	for (i = 0; i < num_ranges; ++i)
		n += ranges[i].num_chars;

	rects = STBTT_malloc(sizeof(*rects) * n, spc->user_allocator_context);
	if (!rects)
		return 0;

	n = stbtt_PackFontRangesGatherRects(spc, info, ranges, num_ranges, rects);

	stbtt_PackFontRangesPackRects(spc, rects, n);

	return_value = stbtt_PackFontRangesRenderIntoRects(spc, info, ranges,
	                                                   num_ranges, rects);

	STBTT_free(rects, spc->user_allocator_context);
	return return_value;
}

static
int rgtt_PackFontRange(stbtt_pack_context *spc, stbtt_fontinfo *info,
                       float font_size, int first_unicode_codepoint_in_range,
                       int num_chars_in_range,
                       stbtt_packedchar *chardata_for_range)
{
	stbtt_pack_range range;
	range.first_unicode_codepoint_in_range = first_unicode_codepoint_in_range;
	range.array_of_unicode_codepoints = NULL;
	range.num_chars                   = num_chars_in_range;
	range.chardata_for_range          = chardata_for_range;
	range.font_size                   = font_size;
	return rgtt_PackFontRanges(spc, info, &range, 1);
}

static
int rgtt_Pack(stbtt_fontinfo *info, int font_size, void *char_info, gui_texture_t *tex)
{
#ifdef __EMSCRIPTEN__
	const s32 bpp = 4;
#else
	const s32 bpp = 1;
#endif
	temp_memory_mark_t mark = temp_memory_save(g_temp_allocator);
	unsigned char *bitmap = NULL;
	s32 w = 512, h = 512;
	stbtt_pack_context context;
	b32 packed = false, failed = false;

	/* TODO(rgriege): oversample with smaller fonts */
	// stbtt_PackSetOversampling(&context, 2, 2);

	while (!packed && !failed) {
		temp_memory_restore(mark);
		bitmap = amalloc(w * h * bpp, g_temp_allocator);
		/* NOTE(rgriege): otherwise bitmap has noise at the bottom */
		memset(bitmap, 0, w * h * bpp);

		if (!stbtt_PackBegin(&context, bitmap, w, h, w * bpp, 1, g_temp_allocator)) {
			failed = true;
		} else if (rgtt_PackFontRange(&context, info, font_size, 0,
		                              info->numGlyphs, char_info)) {
			stbtt_PackEnd(&context); // not really necessary, handled by memory mark
			packed = true;
		} else if (w < 1024) {
			w *= 2;
		} else if (h < 1024) {
			h *= 2;
		} else {
			failed = true; // fail on fonts needing too large a texture
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
		texture_init(tex, w, h, GL_RGBA, bitmap);
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

b32 font_load(font_t *f, const char *filename, u32 sz)
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
	log_debug("packing %d glyphs for %s:%u", f->num_glyphs, filename, sz);
	f->char_info = malloc(f->num_glyphs * sizeof(stbtt_packedchar));

	if (!rgtt_Pack(&info, sz, f->char_info, &f->texture))
		goto err_pack;

	f->filename = filename;
	f->sz = sz;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
	scale = stbtt_ScaleForPixelHeight(&info, sz);
	f->metrics.ascent = scale * ascent;
	f->metrics.descent = scale * descent;
	f->metrics.line_gap = scale * line_gap;
	f->metrics.newline_dist = scale * (ascent - descent + line_gap);
	retval = true;

err_pack:
	if (!retval)
		free(f->char_info);
err_ttf:
	afree(ttf, g_temp_allocator);
out:
	return retval;
}

void font_destroy(font_t *f)
{
	free(f->char_info);
	texture_destroy(&f->texture);
}

typedef enum gui_vbo_type
{
	VBO_VERT,
	VBO_COLOR,
	VBO_TEX,
	VBO_COUNT
} gui_vbo_type_t;

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
#ifdef __EMSCRIPTEN__
	s32 shader_attrib_loc[VBO_COUNT];
#endif
	gui_texture_t texture_white;
	gui_texture_t texture_white_dotted;

	v2i restore_pos;
	v2i restore_dim;

	/* style */
	SDL_Cursor *cursors[GUI_CURSOR_COUNT];
	char font_file_path[256];
	array(font_t) fonts;
	cached_img_t *imgs;

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

void window_restore(window_t *window)
{
	v2i pos, dim;
	SDL_Rect rect;

	if (   window__maximum_window_rect(window, &rect)
	    && (rect.w == window->restore_dim.x && rect.h == window->restore_dim.y)) {
		SDL_GetWindowSize(window->window, &dim.x, &dim.y);
		const v2i border = v2i_scale_inv(dim, 10);
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

static
void *window__get_font(void *handle, u32 sz)
{
	window_t *window = handle;
	font_t *font = window->fonts, *font_end = array_end(window->fonts);
	while (font != font_end && font->sz != sz)
		++font;
	if (font != font_end)
		return font;

	font = array_append_null(window->fonts);
	if (font_load(font, window->font_file_path, sz)) {
		return font;
	} else {
		array_pop(window->fonts);
		assert(false);
		return NULL;
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
	stbtt_aligned_quad q;

	if (codepoint >= font->num_glyphs)
		return false;

	stbtt_GetPackedQuad(font->char_info, font->texture.width, font->texture.height,
	                    codepoint, &x, &y, &q, 1);
	assert(y0 == y);

	/* NOTE(rgriege): stbtt assumes y=0 at top, but for violet y=0 is at bottom */
	quad->texture = font->texture;
	quad->x0 = q.x0;
	quad->y0 = y + (y - q.y1);
	quad->s0 = q.s0;
	quad->t0 = 1.f - q.t0;
	quad->x1 = q.x1;
	quad->y1 = quad->y0 + (q.y1 - q.y0);
	quad->s1 = q.s1;
	quad->t1 = 1.f - q.t1;
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
	const u32 id = hash(fname);
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

window_t *window_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                        window_flags_t flags)
{
	return window_create_ex(x, y, w, h, title, flags, GUI_FONT_FILE_PATH);
}

window_t *window_create_ex(s32 x, s32 y, s32 w, s32 h, const char *title,
                           window_flags_t flags, const char *font_file_path)
{
	window_t *window = calloc(1, sizeof(window_t));
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

#ifndef __EMSCRIPTEN__
	// Use OpenGL 3.3 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif
	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_GetNumVideoDisplays() < 1) {
		log_error("could not create window: no video displays found");
		goto err_win;
	}

	SDL_Rect usable_bounds;
	if (!window__get_display_usable_bounds(0, &usable_bounds))
		goto err_ctx;

	u32 sdl_flags = SDL_WINDOW_OPENGL;
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

	window->gl_context = SDL_GL_CreateContext(window->window);
	if (window->gl_context == NULL) {
		log_error("SDL_CreateContext failed: %s", SDL_GetError());
		goto err_ctx;
	}

	if (SDL_GL_SetSwapInterval(0) != 0)
		log_warn("SDL_GL_SetSwapInterval failed: %s", SDL_GetError());

	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		log_error("glewInit error: %s", glewGetErrorString(glew_err));
		goto err_glew;
	}
	GL_ERR_CHECK("glewInit");

	log_info("GL version: %s", glGetString(GL_VERSION));
	GL_ERR_CHECK("glGetString");

	GL_CHECK(glEnable, GL_MULTISAMPLE);
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
	                                      g_fragment_shader))
		goto err_white;

#ifdef __EMSCRIPTEN__
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
	window->imgs = array_create();

	{
		SDL_Event evt;
		while (SDL_PollEvent(&evt) == 1); /* must be run before SDL_GetWindowSize */
	}

	v2i dim;
	SDL_GetWindowSize(window->window, &dim.x, &dim.y);

	window__store_current_window_rect(window);

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
err_glew:
	SDL_GL_DeleteContext(window->gl_context);
	if (window->parent_window)
		SDL_GL_MakeCurrent(window->parent_window, window->parent_gl_context);
err_ctx:
	SDL_DestroyWindow(window->window);
err_win:
	SDL_Quit();
err_sdl:
	free(window);
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
	free(window);
}

static
void window__drag_cb(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                     s32 offset_x, s32 offset_y, void *udata) {}

void window_drag(window_t *window, s32 x, s32 y, s32 w, s32 h)
{
	gui_t *gui = window->gui;
	gui_style_push(gui, drag, g_gui_style_invis.drag);
	if (gui_drag_rectf(gui, &x, &y, w, h, MB_LEFT, window__drag_cb, NULL)) {
		if (window_is_maximized(window)) {
			/* Shrink the window so that it can be dragged around.
			 * This is going to cause the drag to end, since the drag id won't be the same
			 * next frame.  Fixing this is hard, since the reported mouse position will be
			 * very different next frame even though it hasn't moved. */
			const v2i border = v2i_scale_inv(gui->window_dim, 10);
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
	v2i dim;
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
		}
	}

	SDL_GetWindowSize(window->window, &dim.x, &dim.y);
	gui_event_set_window_dim(gui, dim.x, dim.y);

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
	mouse_pos.y = dim.y - mouse_pos.y;
	gui_event_set_mouse_pos(gui, mouse_pos.x, mouse_pos.y);
	gui_event_set_mouse_btn(gui, mouse_btn);

	keys = SDL_GetKeyboardState(&key_cnt);
	gui_event_set_keyboard(gui, keys, key_cnt);

	gui_events_end(gui);

	GL_CHECK(glViewport, 0, 0, dim.x, dim.y);

	/* NOTE(rgriege): reset the scissor for glClear */
	GL_CHECK(glScissor, 0, 0, dim.x, dim.y);

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
#ifdef __EMSCRIPTEN__
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
	         dim.x/2, dim.y/2);

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

	if (gui_text_input_active(gui) != SDL_IsTextInputActive()) {
		if (gui_text_input_active(gui))
			SDL_StartTextInput();
		else
			SDL_StopTextInput();
	}
}

void window_end_frame_ex(window_t *window, u32 target_frame_milli,
                         u32 idle_frame_milli, u32 idle_start_milli)
{
	gui_t *gui = window->gui;
	u32 frame_milli;

	window_end_frame(window);
	frame_milli = time_diff_milli(gui_frame_start(gui), time_current());

	if (frame_milli > target_frame_milli)
		log_warn("long frame: %ums", frame_milli);
	else if (  time_diff_milli(gui_last_input_time(gui), gui_frame_start(gui))
	         > idle_start_milli)
		time_sleep_milli(idle_frame_milli - frame_milli);
	else
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
		frame_milli = time_diff_milli(gui_frame_start(window->gui), time_current());
		if (frame_milli < target_frame_milli)
			time_sleep_milli(target_frame_milli - frame_milli);
		else
			log_warn("long frame: %ums", frame_milli);
	}
}

#ifdef __EMSCRIPTEN__
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
	"  vec2 TexCoord_flipped = vec2(TexCoord.x, 1.0 - TexCoord.y);\n"
	"  gl_FragColor = texture2D(tex, TexCoord_flipped) * Color;\n"
	"}";
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
	"  vec2 TexCoord_flipped = vec2(TexCoord.x, 1.0 - TexCoord.y);\n"
	"  FragColor = texture(tex, TexCoord_flipped) * Color;\n"
	"}";
#endif // __EMSCRIPTEN__

#endif // SDL_GL_IMPLEMENTATION
