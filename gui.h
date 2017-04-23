#ifndef VIOLET_GUI_H
#define VIOLET_GUI_H

#include "violet/core.h"

typedef struct v2f v2f;

/* Color */

typedef struct color_t
{
	u8 r, g, b, a;
} color_t;

#define gi_black        { .r=0,    .g=0,    .b=0,    .a=0xff }
#define gi_grey77       { .r=0x4d, .g=0x4d, .b=0x4d, .a=0xff }
#define gi_grey128      { .r=0x80, .g=0x80, .b=0x80, .a=0xff }
#define gi_white        { .r=0xff, .g=0xff, .b=0xff, .a=0xff }

#define gi_red          { .r=0xff, .g=0,    .b=0,    .a=0xff }
#define gi_orange       { .r=0xff, .g=0x92, .b=0x1e, .a=0xff }
#define gi_green        { .r=0,    .g=0xff, .b=0,    .a=0xff }
#define gi_lightblue    { .r=0x3f, .g=0xa8, .b=0xf5, .a=0xff }
#define gi_blue         { .r=0,    .g=0,    .b=0xff, .a=0xff }
#define gi_fuchsia      { .r=0xff, .g=0,    .b=0xff, .a=0xff }

#define gi_nocolor      { .r=0,    .g=0,    .b=0,    .a=0 }


#define g_black (color_t)gi_black
#define g_grey77 (color_t)gi_grey77
#define g_grey128 (color_t)gi_grey128
#define g_white (color_t)gi_white

#define g_red (color_t)gi_red
#define g_orange (color_t)gi_orange
#define g_green (color_t)gi_green
#define g_lightblue (color_t)gi_lightblue
#define g_blue (color_t)gi_blue
#define g_fuchsia (color_t)gi_fuchsia

#define g_nocolor (color_t)gi_nocolor

void    color_as_float_array(float *f, color_t c);
color_t color_from_hex(const char *hex);
b32     color_equal(color_t lhs, color_t rhs);


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
void mesh_unbind();
void mesh_set_vertices(mesh_t *m, const v2f *v, u32 n);


/* Texture */

typedef struct texture_t
{
	u32 handle;
	u32 width;
	u32 height;
} texture_t;

b32  texture_load_png(texture_t *tex, const char *filename);
void texture_init(texture_t *tex, u32 w, u32 h, u32 fmt, void *data);
void texture_destroy(texture_t *tex);
void texture_coords_from_poly(mesh_t *tex_coords, const v2f *v, u32 n);
void texture_bind(const texture_t *tex);
void texture_unbind();


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


b32  shader_init(shader_t *shader, const char *filename, shader_type_t type);
void shader_destroy(shader_t *shader);

typedef struct shader_prog_t
{
	u32 handle;
	shader_t vertex_shader;
	shader_t frag_shader;
} shader_prog_t;

b32  shader_program_load(shader_prog_t *prog, const char *name);
b32  shader_program_create(shader_prog_t *prog, shader_t vertex_shader,
                           shader_t frag_shader);
void shader_program_destroy(shader_prog_t *p);
void shader_program_bind(const shader_prog_t *p);
void shader_program_unbind();
s32  shader_program_attrib(const shader_prog_t *p, const char *name);
s32  shader_program_uniform(const shader_prog_t *p, const char *name);


/* Image */

typedef struct img_t
{
	u32 vao;
	texture_t texture;
	mesh_t mesh;
	mesh_t tex_coords;
} img_t;

b32  img_load(img_t *img, const char *filename);
void img_init(img_t *img, u32 w, u32 h, u32 fmt, void *data);
void img_destroy(img_t *img);
void img_render(const img_t *img, s32 x, s32 y, r32 sx, r32 sy, shader_prog_t *p);


/* Font */

typedef enum font_align_t
{
	FONT_ALIGN_LEFT,
	FONT_ALIGN_CENTER,
	FONT_ALIGN_RIGHT
} font_align_t;

typedef struct glyph_t
{
	char charcode;
	u32 vao;
	texture_t texture;
	mesh_t mesh;
	mesh_t tex_coords;
	s32 offset_x, offset_y;
	u32 advance;
} glyph_t;

typedef struct font_t
{
	const char *filename;
	u32 sz;
	glyph_t *glyphs;
	u32 space_width;
	u32 newline_dist;
} font_t;

b32     font_install();
b32     font_uninstall();
b32     font_load(font_t *f, const char *filename, u32 sz);
void    font_destroy(font_t *f);
void    font_render(font_t *f, const char *txt, s32 x, s32 y,
                    shader_prog_t *p, font_align_t align);
void    font_render_ex(font_t *f, const char *txt, s32 *x, s32 *y,
                       shader_prog_t *p, font_align_t align);
void    font_bounds(font_t *f, const char *txt, s32 *x, s32 *y,
                    font_align_t align);


/* General Gui */

typedef struct gui_t gui_t;

typedef enum gui_flags_t
{
	WINDOW_BORDERLESS = 1 << 0,
	WINDOW_RESIZABLE = 1 << 1
} gui_flags_t;

gui_t *gui_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                  gui_flags_t flags);
void   gui_destroy(gui_t *gui);
void   gui_dim(const gui_t *gui, s32 *x, s32 *y);
void   gui_minimize(gui_t *gui);
b32    gui_begin_frame(gui_t *gui);
void   gui_end_frame(gui_t *gui);
void   gui_run(gui_t *gui, u32 fps, b32(*ufunc)(gui_t *gui, void *udata),
               void *udata);


typedef enum mouse_button_t
{
	MB_LEFT = 1 << 0,
	MB_MIDDLE = 1 << 1,
	MB_RIGHT = 1 << 2,
	MB_X1 = 1 << 3,
	MB_X2 = 1 << 4,
	MB_WHEELUP = 1 << 5,
	MB_WHEELDOWN = 1 << 6
} mouse_button_t;

/* Input */

void mouse_pos(const gui_t *gui, s32 *x, s32 *y);
b32  mouse_press(const gui_t *gui, u32 mask);
b32  mouse_down(const gui_t *gui, u32 mask);
b32  mouse_release(const gui_t *gui, u32 mask);
b32  mouse_release_bg(const gui_t *gui, u32 mask);
void mouse_scroll(const gui_t *gui, s32 *dir);

char key_press(const gui_t *gui);
char key_down(const gui_t *gui);
char key_release(const gui_t *gui);


/* Retained Mode API */

typedef struct rmgui_poly_t
{
	mesh_t mesh;
	u32 vao;
	color_t fill_color;
	color_t line_color;
} rmgui_poly_t;

void rmgui_poly_init(gui_t *gui, const v2f *v, u32 n, mesh_t *mesh, u32 *vao);
void rmgui_line_init(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                     s32 w, color_t c, rmgui_poly_t *line);
void rmgui_rect_init(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     color_t fill, color_t line, rmgui_poly_t *rect);
void rmgui_circ_init(gui_t *gui, s32 x, s32 y, s32 r, color_t fill,
                     color_t line, rmgui_poly_t *circ);
void rmgui_poly_destroy(rmgui_poly_t *poly);
void rmgui_poly_draw(gui_t *gui, const rmgui_poly_t *poly, s32 x, s32 y);

void rmgui_img_draw(gui_t *gui, const img_t *img, s32 x, s32 y);
void rmgui_img_draw_scaled(gui_t *gui, const img_t *img, s32 x, s32 y,
                           r32 sx, r32 sy);


/* Immediate Mode API */

/* Primitives */

void gui_line(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w, color_t c);
void gui_rect(gui_t *gui, s32 x, s32 y, s32 w, s32 h, color_t fill, color_t line);
void gui_circ(gui_t *gui, s32 x, s32 y, s32 r, color_t fill, color_t line);
void gui_poly(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t line);
void gui_img(gui_t *gui, s32 x, s32 y, const char *img);
void gui_txt(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt, color_t c,
             font_align_t align);
void gui_mask(gui_t *gui, s32 x, s32 y, s32 w, s32 h);
void gui_unmask(gui_t *gui);

/* Widgets */

typedef enum btn_val_t
{
	BTN_NONE,
	BTN_PRESS,
	BTN_HOLD
} btn_val_t;

typedef enum npt_flags_t
{
	NPT_PASSWORD = 1 << 0,
	NPT_NUMERIC  = 1 << 1,
} npt_flags_t;

b32       gui_npt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                  font_align_t align, npt_flags_t flags);
btn_val_t gui_btn(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt);
void      gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt,
                  b32 *val);
void      gui_slider(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val);
void      gui_select(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     const char *txt, u32 *val, u32 opt);
void      gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                      const char *txt, u32 *val, u32 opt);
b32       gui_drag(gui_t *gui, s32 *x, s32 *y, u32 w, u32 h, mouse_button_t mb);
b32       gui_cdrag(gui_t *gui, s32 *x, s32 *y, u32 r, mouse_button_t mb);
void      gui_scrollbar(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                        s32 ttl_dim, r32 *off);


/* Style */

typedef struct gui_style_t
{
	color_t bg_color;
	color_t fill_color;
	color_t hot_color;
	color_t active_color;
	color_t outline_color;
	color_t hot_line_color;
	color_t active_line_color;
	color_t text_color;
	color_t hot_text_color;
	color_t active_text_color;
	r32 font_ratio;
} gui_style_t;

const gui_style_t *gui_get_style(gui_t *gui);
void               gui_style(gui_t *gui, const gui_style_t *style);
void               gui_style_default(gui_t *gui);

#endif // VIOLET_GUI_H


/* Implementation */

#ifdef GUI_IMPLEMENTATION

#include <ctype.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <math.h>
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "violet/array.h"
#include "violet/fmath.h"
#include "violet/imath.h"


/* Color */

void color_as_float_array(float *f, color_t c)
{
	f[0] = c.r / 255.f;
	f[1] = c.g / 255.f;
	f[2] = c.b / 255.f;
	f[3] = c.a / 255.f;
}

color_t color_from_hex(const char *hex)
{
	color_t c = g_nocolor;
	u32 val;

	if (*hex == '#')
		++hex;

	val = strtoul(hex, NULL, 16);
	switch (strlen(hex)) {
	case 6:
		// no alpha
		c.r = (val >> 16) & 0xff;
		c.g = (val >> 8) & 0xff;
		c.b = val & 0xff;
		c.a = 255;
		break;
	case 8:
		// with alpha
		c.r = (val >> 24) & 0xff;
		c.g = (val >> 16) & 0xff;
		c.b = (val >> 8) & 0xff;
		c.a = val & 0xff;
		break;
	default:
		log_write("invalid color string '%s'", hex);
		break;
	}

	return c;
}

b32 color_equal(color_t lhs, color_t rhs)
{
	return memcmp(&lhs, &rhs, sizeof(color_t)) == 0;
}


/* Mesh */

void mesh_init(mesh_t *m, const v2f *poly, u32 n)
{
	glGenBuffers(1, &m->vbo);
	mesh_set_vertices(m, poly, n);
}

void mesh_destroy(mesh_t *m)
{
	if (m->vbo != 0)
		glDeleteBuffers(1, &m->vbo);
}

void mesh_poly(const mesh_t *m, v2f *poly)
{
	array_clear(poly);
	array_reserve(poly, m->sz);
	array_sz(poly) = m->sz;
	mesh_bind(m);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, m->sz * 2 * sizeof(GL_FLOAT), poly);
}

void mesh_bind(const mesh_t *m)
{
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
}

void mesh_unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void mesh_set_vertices(mesh_t *m, const v2f *v, u32 n)
{
	mesh_bind(m);
	m->sz = n;
	glBufferData(GL_ARRAY_BUFFER, m->sz * 2 * sizeof(GL_FLOAT), v, GL_STATIC_DRAW);
}

/* Texture */

b32 texture_load_png(texture_t *tex, const char *filename)
{
	b32 ret = false;
#if PNG_LIBPNG_VER_MINOR == 6 // 1.6
	png_image image = {0};
	png_bytep buf;

	image.version = PNG_IMAGE_VERSION;
	if (png_image_begin_read_from_file(&image, filename)) {
		image.format = PNG_FORMAT_RGBA;
		buf = malloc(PNG_IMAGE_SIZE(image));
		if (buf) {
			if (png_image_finish_read(&image, NULL, buf, 0, NULL)) {
				texture_init(tex, image.width, image.height, GL_RGBA, buf);
				ret = true;
			} else {
				log_write("png '%s' read: %s", filename, image.message);
				png_image_free(&image);
			}
			free(buf);
		} else {
			log_write("png read: out of memory");
		}
	}
#else // fallback to 1.2
	FILE *fp;
	png_byte header[8];
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	int bit_depth, color_type;
	GLint format;
	int bytes_per_row;
	png_bytep image_data, *row_pointers;

	if (!(fp = fopen(filename, "rb")))
		goto out;

	if (fread(header, 1, 8, fp) != 8)
		goto err_png;
	if (png_sig_cmp(header, 0, 8))
		goto err_png;

	if (!(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
		goto err_png;

	if (!(info_ptr = png_create_info_struct(png_ptr))) {
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		goto err_png;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
		goto err;

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	             NULL, NULL, NULL);

	if (bit_depth != 8)
		goto err;

	switch (color_type) {
		case PNG_COLOR_TYPE_RGB:
			format = GL_RGB;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			format = GL_RGBA;
			break;
		default:
			goto err;
	}

	png_read_update_info(png_ptr, info_ptr);

	bytes_per_row = png_get_rowbytes(png_ptr, info_ptr);
	bytes_per_row += 3 - ((bytes_per_row - 1) % 4); // 4-byte aligned

	image_data = malloc(bytes_per_row * height * sizeof(png_byte) + 15);
	row_pointers = malloc(height * sizeof(png_bytep));
	for (u32 i = 0; i < height; ++i)
		row_pointers[i] = image_data + i * bytes_per_row;

	png_read_image(png_ptr, row_pointers);

	texture_init(tex, width, height, format, image_data);
	ret = true;

	free(row_pointers);
	free(image_data);

err:
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
err_png:
	fclose(fp);
out:
#endif // PNG_LIBPNG_VER_MINOR
	return ret;
}

void texture_init(texture_t *tex, u32 w, u32 h, u32 fmt, void *data)
{
	glGenTextures(1, &tex->handle);
	tex->width = w;
	tex->height = h;
	texture_bind(tex);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture_unbind();
}

void texture_destroy(texture_t *tex)
{
	if (tex->handle != 0)
		glDeleteTextures(1, &tex->handle);
}

void texture_coords_from_poly(mesh_t *tex_coords, const v2f *v, u32 n)
{
	box2f extent;
	v2f dimension;
	v2f *coords = array_create();

	polyf_bounding_box(v, n, &extent);
	dimension = v2f_sub(extent.max, extent.min);

	array_reserve(coords, n);
	for (const v2f *vn=v+n; v!=vn; ++v)
		array_append(coords, v2f_div(v2f_sub(*v, extent.min), dimension));
	mesh_init(tex_coords, coords, n);
	array_destroy(coords);
}

void texture_bind(const texture_t *tex)
{
	glBindTexture(GL_TEXTURE_2D, tex->handle);
}

void texture_unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}


/* Shader */

b32 shader_init(shader_t *shader, const char *filename, shader_type_t type)
{
	b32 retval = false;
	FILE *file;
	char *file_buf, *log_buf;
	size_t fsize;
	GLint compiled, log_len;

	file = fopen(filename, "r");
	if (!file) {
		log_write("Could not open shader file '%s'", filename);
		return retval;
	}

	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	rewind(file);
	file_buf = malloc(fsize + 1);
	if (fread(file_buf, 1, fsize, file) != fsize) {
		log_write("Failed to read shader file '%s'", filename);
		goto err;
	}
	file_buf[fsize] = 0;

	shader->handle = glCreateShader(type == VERTEX_SHADER ?
	                                GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(shader->handle, 1, (const GLchar **)&file_buf, 0);

	glCompileShader(shader->handle);
	glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		glGetShaderiv(shader->handle, GL_INFO_LOG_LENGTH, &log_len);
		log_buf = malloc(log_len);
		glGetShaderInfoLog(shader->handle, log_len, NULL, log_buf);
		log_write("Compilation error in shader file '%s': %s", filename, log_buf);
		free(log_buf);
		shader->handle = 0;
		goto err;
	}

	retval = true;
err:
	free(file_buf);
	fclose(file);
	return retval;
}

void shader_destroy(shader_t *shader)
{
	glDeleteShader(shader->handle);
	shader->handle = 0;
}

static
b32 shader_program__load_internal(shader_prog_t *p,
                                  const char *vertex_shader_filename,
                                  const char *frag_shader_filename)
{
	b32 retval = false;
	if (!shader_init(&p->vertex_shader, vertex_shader_filename, VERTEX_SHADER))
		goto out;

	if (!shader_init(&p->frag_shader, frag_shader_filename, FRAGMENT_SHADER))
		goto err_frag;
	
	if (shader_program_create(p, p->vertex_shader, p->frag_shader)) {
		retval = true;
		goto out;
	}

	shader_destroy(&p->frag_shader);
err_frag:
	shader_destroy(&p->vertex_shader);
out:
	return retval;
}

b32 shader_program_load(shader_prog_t *p, const char *name)
{
	b32 retval;
	u32 name_len;
	char *vertex_shader_name, *frag_shader_name;

	name_len = strlen(name);

	vertex_shader_name = malloc(name_len + 6);
	strcpy(vertex_shader_name, name);
	strcpy(vertex_shader_name + name_len, ".vert");

	frag_shader_name = malloc(name_len + 6);
	strcpy(frag_shader_name, name);
	strcpy(frag_shader_name + name_len, ".frag");

	retval = shader_program__load_internal(p, vertex_shader_name, frag_shader_name);

	free(vertex_shader_name);
	free(frag_shader_name);

	return retval;
}

b32 shader_program_create(shader_prog_t *p, shader_t vertex_shader,
                          shader_t frag_shader)
{
	GLint linked, length;
	char *log_buf;

	p->handle = glCreateProgram();

	glAttachShader(p->handle, vertex_shader.handle);
	p->vertex_shader = vertex_shader;
	glAttachShader(p->handle, frag_shader.handle);
	p->frag_shader = frag_shader;

	glLinkProgram(p->handle);
	glGetProgramiv(p->handle, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE) {
		glGetProgramiv(p->handle, GL_INFO_LOG_LENGTH, &length);
		log_buf = malloc(length);
		glGetProgramInfoLog(p->handle, length, NULL, log_buf);
		log_write("Shader link error using '%s' & '%s': %s",
		          vertex_shader.filename, frag_shader.filename, log_buf);
		free(log_buf);
		return false;
	}

	return true;
}

void shader_program_bind(const shader_prog_t *p)
{
	glUseProgram(p->handle);
}

void shader_program_unbind()
{
	glUseProgram(0);
}

void shader_program_destroy(shader_prog_t *p)
{
	glDetachShader(p->handle, p->vertex_shader.handle);
	shader_destroy(&p->vertex_shader);
	glDetachShader(p->handle, p->frag_shader.handle);
	shader_destroy(&p->frag_shader);
	glDeleteProgram(p->handle);
	p->handle = 0;
}

s32 shader_program_attrib(const shader_prog_t *p, const char *name)
{
	return glGetAttribLocation(p->handle, name);
}

s32 shader_program_uniform(const shader_prog_t *p, const char *name)
{
	return glGetUniformLocation(p->handle, name);
}


/* Image */

static
void img__init_complete(img_t *img)
{
	box2f box;
	v2f poly[4];

	box2f_from_dims(&box, 0, img->texture.height, img->texture.width, 0);
	polyf_from_box(poly, box);

	mesh_init(&img->mesh, poly, 4);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	texture_coords_from_poly(&img->tex_coords, poly, 4);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);
}

b32 img_load(img_t *img, const char *filename)
{
	glGenVertexArrays(1, &img->vao);
	glBindVertexArray(img->vao);

	if (!texture_load_png(&img->texture, filename)) {
		log_write("img_load(%s) error", filename);
		return false;
	}

	img__init_complete(img);

	glBindVertexArray(0);
	return true;
}

void img_init(img_t *img, u32 w, u32 h, u32 fmt, void *data)
{
	glGenVertexArrays(1, &img->vao);
	glBindVertexArray(img->vao);

	texture_init(&img->texture, w, h, fmt, data);
	img__init_complete(img);

	glBindVertexArray(0);
}

void img_destroy(img_t *img)
{
	mesh_destroy(&img->mesh);
	mesh_destroy(&img->tex_coords);
	texture_destroy(&img->texture);
	glDeleteVertexArrays(1, &img->vao);
}

void img_render(const img_t *img, s32 x, s32 y, r32 sx, r32 sy, shader_prog_t *p)
{
	glBindVertexArray(img->vao);
	texture_bind(&img->texture);

	glUniform2f(shader_program_uniform(p, "offset"), x, y);
	glUniform2f(shader_program_uniform(p, "scale"), sx, sy);
	glDrawArrays(GL_TRIANGLE_FAN, 0, img->mesh.sz);

	texture_unbind();
	glBindVertexArray(0);
}


/* Font */

FT_Library g_freetype_lib = {0};

b32 font_install()
{
	assert(!g_freetype_lib);
	FT_Error err = FT_Init_FreeType(&g_freetype_lib);
	if (err != FT_Err_Ok) {
		log_write("FT_Init_FreeType error: %d", err);
		return false;
	}
	return true;
}

b32 font_uninstall()
{
	assert(g_freetype_lib);
	FT_Error err = FT_Done_FreeType(g_freetype_lib);
	if (err != FT_Err_Ok) {
		log_write("FT_Done_FreeType error: %d", err);
		return false;
	}
	g_freetype_lib = NULL;
	return true;
}

static
void glyph__render(glyph_t *g, shader_prog_t *p)
{
	glBindVertexArray(g->vao);
	texture_bind(&g->texture);

	glDrawArrays(GL_TRIANGLE_FAN, 0, g->mesh.sz);

	texture_unbind();
	glBindVertexArray(0);
}

static
void glyph__destroy(glyph_t *g)
{
	mesh_destroy(&g->mesh);
	mesh_destroy(&g->tex_coords);
	texture_destroy(&g->texture);
	glDeleteVertexArrays(1, &g->vao);
}

static
int glyph__sort(const void *_lhs, const void *_rhs)
{
	const glyph_t *lhs = _lhs;
	const glyph_t *rhs = _rhs;
	return lhs->charcode - rhs->charcode;
}

static
glyph_t *glyph__find(glyph_t *glyphs, char charcode)
{
	glyph_t *g = glyphs, *gn = array_end(glyphs);
	while (g->charcode < charcode && g != gn)
		++g;
	return g != gn && g->charcode == charcode ? g : NULL;
}

b32 font_load(font_t *f, const char *filename, u32 sz)
{
	b32 retval = false;

	FT_Face face;
	FT_Error err = FT_New_Face(g_freetype_lib, filename, 0, &face);
	if (err != FT_Err_Ok) {
		log_write("FT_New_Face error: %d", err);
		return retval;
	}

	err = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	if (err != FT_Err_Ok) {
		log_write("FT_Select_Charmap error: %d", err);
		goto err_face;
	}

	err = FT_Set_Char_Size(face, 0, sz * 64, 0, 0);
	if (err != FT_Err_Ok) {
		log_write("FT_Set_Char_Size error: %d", err);
		goto err_face;
	}

	f->filename = filename;
	f->sz = sz;
	f->space_width = sz; // default
	f->newline_dist = face->height >> 6;
	f->glyphs = array_create();
	array_reserve(f->glyphs, 128);


	FT_ULong charcode;
	FT_UInt glyph_idx;
	charcode = FT_Get_First_Char(face, &glyph_idx);
	u32 pixel_buf_sz = 16*16;
	u8 *pixels = malloc(pixel_buf_sz);
	s32 min_y = 0;
	while (glyph_idx != 0) {
		if (charcode > 127) {
			charcode = FT_Get_Next_Char(face, charcode, &glyph_idx);
			continue;
		}

		err = FT_Load_Glyph(face, glyph_idx, FT_LOAD_RENDER);
		if (err != FT_Err_Ok) {
			charcode = FT_Get_Next_Char(face, charcode, &glyph_idx);
			log_write("FT_Load_Glyph(%s, %lu) error: 0x%x", filename, charcode, err);
			continue;
		}

		const FT_Bitmap bitmap = face->glyph->bitmap;
		if (bitmap.buffer) {
			glyph_t *glyph = array_append_null(f->glyphs);
			glyph->charcode = charcode;
			// NOTE(rgriege): having issues with textures smaller than this
			const u32 tex_height = max(16u,pow(2,ceil(log2(bitmap.rows))));
			const u32 tex_width = max(16u,pow(2,ceil(log2(bitmap.width))));
			const u32 tex_sz = tex_width*tex_height;
			if (tex_sz > pixel_buf_sz) {
				free(pixels);
				pixels = malloc(tex_sz);
			}

			for (int i = 0; i < bitmap.rows; ++i) {
				memcpy(pixels+i*tex_width, bitmap.buffer+i*bitmap.width, bitmap.width);
				memset(pixels+i*tex_width+bitmap.width, 0, tex_width-bitmap.width);
			}
			memset(pixels+bitmap.rows*tex_width, 0, tex_width*(tex_height-bitmap.rows));

			glGenVertexArrays(1, &glyph->vao);
			glBindVertexArray(glyph->vao);

			texture_init(&glyph->texture, tex_width, tex_height, GL_RED, pixels);

			v2f vertices[4] = {
				{ .x=0, .y=0 },
				{ .x=0, .y=tex_height },
				{ .x=tex_width, .y=tex_height },
				{ .x=tex_width, .y=0 }
			};
			mesh_init(&glyph->mesh, vertices, 4);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(0);

			
			v2f coords[4] = {
				{ .x=0, .y=0 },
				{ .x=0, .y=1 },
				{ .x=1, .y=1 },
				{ .x=1, .y=0 }
			};
			mesh_init(&glyph->tex_coords, coords, 4);
			glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(1);

			glBindVertexArray(0);

			glyph->offset_x = face->glyph->bitmap_left;
			glyph->offset_y = face->glyph->bitmap_top - tex_height;
			const s32 y = face->glyph->bitmap_top - bitmap.rows;
			if (y < min_y)
				min_y = y;
			glyph->advance = face->glyph->advance.x >> 6;
		}
		else if (charcode == 32) {
			f->space_width = face->glyph->advance.x >> 6;
		} else {
			log_write("Charcode has no bitmap: %lu", charcode);
		}

		charcode = FT_Get_Next_Char(face, charcode, &glyph_idx);
	}

	array_foreach(f->glyphs, glyph_t, g)
		g->offset_y -= min_y;
	array_qsort(f->glyphs, glyph__sort);

	free(pixels);
	retval = true;

err_face:
	err = FT_Done_Face(face);
	if (err != FT_Err_Ok)
		log_write("FT_Done_Face error: %d", err);
	return retval;
}

void font_destroy(font_t *f)
{
	array_foreach(f->glyphs, glyph_t, g)
		glyph__destroy(g);
	array_destroy(f->glyphs);
}

static
r32 font__line_offset_x(font_t *f, const char *txt, font_align_t align)
{
	r32 width = 0.f, factor = 0.f; // FONT_ALIGN_LEFT by default
	switch (align) {
	case FONT_ALIGN_LEFT:
		break;
	case FONT_ALIGN_CENTER:
		factor = 0.5f;
		break;
	case FONT_ALIGN_RIGHT:
		factor = 1.f;
		break;
	}
	for (u32 i = 0, end = strlen(txt); i < end; ++i) {
		const char character = txt[i];
		switch (character) {
		case ' ':
			width += f->space_width;
			break;
		case '\r':
			goto out;
			break;
		default: {
			glyph_t *glyph = glyph__find(f->glyphs, character);
			if (glyph)
				width += glyph->advance;
			else
				log_write("unknown character: '%u'", character);
		}
			break;
		}
	}
out:
	return width * factor;
}

void font_render(font_t *f, const char *txt, s32 x, s32 y,
                 shader_prog_t *p, font_align_t align)
{
	font_render_ex(f, txt, &x, &y, p, align);
}

void font_render_ex(font_t *f, const char *txt, s32 *x, s32 *y, shader_prog_t *p,
                    font_align_t align)
{
	const s32 x_orig = *x;
	s32 off = font__line_offset_x(f, txt, align);
	for (u32 i = 0, end = strlen(txt); i < end; ++i) {
		const char character = txt[i];
		switch (character) {
		case ' ':
			*x += f->space_width;
			break;

		case '\r':
			*y -= f->newline_dist;
			*x = x_orig;
			if (i < end - 1)
				off = font__line_offset_x(f, txt + i + 1, align);
			break;

		default: {
			glyph_t *glyph = glyph__find(f->glyphs, character);
			if (glyph) {
				// looks bad if offset not rounded
				const GLint offset_attrib = shader_program_uniform(p, "offset");
				glUniform2f(offset_attrib, *x + glyph->offset_x - off,
				            *y + glyph->offset_y);
				glyph__render(glyph, p);
				*x += glyph->advance;
			}
		}
			break;
		}
	}
	*x -= off;
}

void font_bounds(font_t *f, const char *txt, s32 *x, s32 *y, font_align_t align)
{
	const s32 x_orig = *x;
	for (u32 i = 0, end = strlen(txt); i < end; ++i) {
		const char character = txt[i];
		switch (character) {
		case ' ':
			*x += f->space_width;
			break;

		case '\r':
			*y -= f->newline_dist;
			*x = x_orig;
			break;

		default: {
			glyph_t *glyph = glyph__find(f->glyphs, character);
			if (glyph)
				*x += glyph->advance;
		}
		break;
		}
	}
}


/*
 * Polygon Decomposition
 * Translated from Mark Bayazit's C++ decomposition algorithm.
 * https://mpen.ca/406/bayazit
 */

static inline
r32 tri_area(v2f a, v2f b, v2f c)
{
	return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
}

static inline
b32 tri_left(v2f a, v2f b, v2f c)
{
	return tri_area(a, b, c) > 0;
}

static inline
b32 tri_left_on(v2f a, v2f b, v2f c)
{
	return tri_area(a, b, c) >= 0;
}

static inline
b32 tri_right(v2f a, v2f b, v2f c)
{
	return tri_area(a, b, c) < 0;
}

static inline
b32 tri_right_on(v2f a, v2f b, v2f c)
{
	return tri_area(a, b, c) <= 0;
}

static inline
b32 tri_is_reflex(v2f a, v2f b, v2f c)
{
	return tri_right(a, b, c);
}

static
void polyf_decompose(const v2f *v, u32 n, v2f ***polys)
{
	assert(polyf_is_cc(v, n));

	v2f upper_int={0}, lower_int={0}, p;
	r32 upper_dist, lower_dist, d, closest_dist;
	u32 upper_idx=0, lower_idx=0, closest_idx=0;
	v2f *lower_poly = array_create();
	v2f *upper_poly = array_create();

	v2f vi0 = v[n-1];
	for (u32 i = 0; i < n; ++i) {
		v2f vi1 = v[i];
		v2f vi2 = v[(i+1)%n];
		if (!tri_is_reflex(vi0, vi1, vi2))
			goto inxt;

		upper_dist = lower_dist =  FLT_MAX;
		v2f vj0 = v[n-1];
		for (u32 j = 0; j < n; ++j) {
			v2f vj1 = v[j];
			v2f vj2 = v[(j+1)%n];
			if (tri_left(vi0, vi1, vj1) && tri_right_on(vi0, vi1, vj0)) {
				check(fmath_line_intersect(vi0, vi1, vj0, vj1, &p));
				if (tri_right(vi2, vi1, p)) {
					d = v2f_dist_sq(vi1, p);
					if (d < lower_dist) {
						lower_dist = d;
						lower_int = p;
						lower_idx = j;
					}
				}
			}
			if (tri_left(vi2, vi1, vj2) && tri_right_on(vi2, vi1, vj1)) {
				check(fmath_line_intersect(vi2, vi1, vj1, vj2, &p));
				if (tri_left(vi0, vi1, p)) {
					d = v2f_dist_sq(vi1, p);
					if (d < upper_dist) {
						upper_dist = d;
						upper_int = p;
						upper_idx = j;
					}
				}
			}
			vj0 = vj1;
		}

		if (lower_idx == (upper_idx+1)%n) {
			// case 1: no vertices to connect to, choose a point in the middle
			p.x = (lower_int.x+upper_int.x)/2;
			p.y = (lower_int.y+upper_int.y)/2;

			if (i < upper_idx) {
				for (u32 j = i; j <= upper_idx; ++j)
					array_append(lower_poly, v[j]);
				array_append(lower_poly, p);
				array_append(upper_poly, p);
				if (lower_idx != 0)
					for (u32 j = lower_idx; j < n; ++j)
						array_append(upper_poly, v[j]);
				for (u32 j = 0; j <= i; ++j)
					array_append(upper_poly, v[j]);
			} else {
				if (i != 0)
					for (u32 j = i; j < n; ++j)
						array_append(lower_poly, v[j]);
				for (u32 j = 0; j <= upper_idx; ++j)
					array_append(lower_poly, v[j]);
				array_append(lower_poly, p);
				array_append(upper_poly, p);
				for (u32 j = lower_idx; j <= i; ++j)
					array_append(upper_poly, v[j]);
			}
		} else {
			// case 2: connect to closest point within the triangle
			if (lower_idx > upper_idx)
				upper_idx += n;
			closest_dist = FLT_MAX;

			for (u32 j = lower_idx; j <= upper_idx; ++j) {
				v2f vj = j < 0 ? v[j%n+n] : v[j%n];
				if (tri_left_on(vi0, vi1, vj) && tri_right_on(vi2, vi1, vj)) {
					d = v2f_dist_sq(vi1, vj);
					if (d < closest_dist) {
						closest_dist = d;
						closest_idx = j % n;
					}
				}
			}

			if (i < closest_idx) {
				for (u32 j = i; j <= closest_idx; ++j)
					array_append(lower_poly, v[j]);
				if (closest_idx != 0)
					for (u32 j = closest_idx; j < n; ++j)
						array_append(upper_poly, v[j]);
				for (u32 j = 0; j <= i; ++j)
					array_append(upper_poly, v[j]);
			} else {
				if (i != 0)
					for (u32 j = i; j < n; ++j)
						array_append(lower_poly, v[j]);
				for (u32 j = 0; j <= closest_idx; ++j)
					array_append(lower_poly, v[j]);
				for (u32 j = closest_idx; j <= i; ++j)
					array_append(upper_poly, v[j]);
			}
		}

		// solve smallest poly first
		if (array_sz(lower_poly) < array_sz(upper_poly)) {
			polyf_decompose(lower_poly, array_sz(lower_poly), polys);
			polyf_decompose(upper_poly, array_sz(upper_poly), polys);
		} else {
			polyf_decompose(upper_poly, array_sz(upper_poly), polys);
			polyf_decompose(lower_poly, array_sz(lower_poly), polys);
		}
		goto out;

inxt:
		vi0 = vi1;
	}

	v2f **poly = array_append_null(*polys);
	*poly = array_create();
	array_reserve(*poly, n);
	array_sz(*poly) = n;
	memcpy(*poly, v, n*sizeof(v2f));

out:
	array_destroy(lower_poly);
	array_destroy(upper_poly);
}


/* General Gui */

static const gui_style_t g_default_style = {
	.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff },
	.fill_color = gi_grey77,
	.hot_color = { .r=0x66, .g=0x66, .b=0x66, .a=0xff },
	.active_color = gi_orange,
	.outline_color = gi_nocolor,
	.hot_line_color = gi_nocolor,
	.active_line_color = gi_nocolor,
	.text_color = gi_white,
	.hot_text_color = gi_white,
	.active_text_color = gi_white,
	.font_ratio = 0.6f
};


static
int gui__sdl_evt_filter(void *userdata, SDL_Event *event)
{
	return event->type == SDL_QUIT || event->type == SDL_MOUSEWHEEL;
}

typedef enum special_key_t
{
	KEY_RETURN = 13,
	KEY_BACKSPACE = 8
} special_key_t;

static const char g_caps[128] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
	 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
	 20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
	 30,  31,  32,  33,  34,  35,  36,  37,  38,  34,
	 40,  41,  42,  43,  60,  95,  62,  63,  41,  33,
	 64,  35,  36,  37,  94,  38,  42,  40,  58,  58,
	 60,  43,  62,  63,  64,  65,  66,  67,  68,  69,
	 70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
	 90, 123, 124, 125,  94,  95, 126,  65,  66,  67,
	 68,  69,  70,  71,  72,  73,  74,  75,  76,  77,
	 78,  79,  80,  81,  82,  83,  84,  85,  86,  87,
	 88,  89,  90, 123, 124, 125, 126, 127
};

static inline
b32 gui__convert_scancode(SDL_Scancode code, b32 caps, char *c)
{
	const SDL_Keycode key = SDL_GetKeyFromScancode(code);
	if (key < 128) {
		if (caps)
			*c = g_caps[key];
		else
			*c = key;
	} else if (key >= SDLK_KP_DIVIDE && key <= SDLK_KP_PERIOD) {
		static char keys[1 + SDLK_KP_PERIOD - SDLK_KP_DIVIDE] = {
			'/', '*', '-', '+', KEY_RETURN, '1', '2', '3', '4', '5', '6',
			'7', '8', '9', '0', '.'
		};
		*c = keys[key - SDLK_KP_DIVIDE];
	} else {
		return false;
	}

	return true;
}

typedef struct gui_t
{
	timepoint_t creation_time;
	timepoint_t frame_start_time;
	u32 frame_time_milli;
	SDL_Window *window;
	SDL_GLContext gl_context;
	shader_prog_t poly_shader;
	shader_prog_t tex_shader;
	shader_prog_t txt_shader;
	v2i win_halfdim;
	v2i mouse_pos;
	u32 mouse_btn;
	u32 mouse_btn_diff;
	char prev_key, key;
	u32 repeat_delay;
	u32 repeat_interval;
	u32 repeat_timer;
	font_t *fonts;
	gui_style_t default_style;
	gui_style_t style;
	u64 hot_id;
	u64 active_id;
	u64 active_id_at_frame_start;
	v2i drag_offset;
	char *pw_buf;
} gui_t;

gui_t *gui_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                  gui_flags_t flags)
{
	gui_t *gui = calloc(1, sizeof(gui_t));
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_write("SDL_Init(VIDEO) failed: %s", SDL_GetError());
		goto err_sdl;
	}

	// Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_GetNumVideoDisplays() < 1) {
		log_write("could not create window: no video displays found");
		goto err_win;
	}

	SDL_DisplayMode display_mode;
	if (SDL_GetCurrentDisplayMode(0, &display_mode) != 0) {
		log_write("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
		goto err_win;
	}

	u32 sdl_flags = SDL_WINDOW_OPENGL;
	s32 min_h = display_mode.h - 60;
	if (flags & WINDOW_BORDERLESS) {
		sdl_flags |= SDL_WINDOW_BORDERLESS;
		min_h += 30;
	}
	if (flags & WINDOW_RESIZABLE)
		sdl_flags |= SDL_WINDOW_RESIZABLE;
#ifdef _WIN32
	gui->window = SDL_CreateWindow(title, x, y, min(w, display_mode.w),
	                               min(h, min_h), sdl_flags);
#else
	gui->window = SDL_CreateWindow(title, x, max(y, 30), min(w, display_mode.w),
	                               min(h, min_h), sdl_flags);
#endif
	if (gui->window == NULL) {
		log_write("SDL_CreateWindow failed: %s", SDL_GetError());
		goto err_win;
	}

	gui->gl_context = SDL_GL_CreateContext(gui->window);
	if (gui->gl_context == NULL) {
		log_write("SDL_CreateContext failed: %s", SDL_GetError());
		goto err_ctx;
	}

	SDL_SetEventFilter(gui__sdl_evt_filter, NULL);

	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		log_write("glewInit error: %s", glewGetErrorString(glew_err));
		goto err_glew;
	}

	log_write("GL version: %s", glGetString(GL_VERSION));

	glEnable(GL_MULTISAMPLE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_SCISSOR_TEST);

	if (!shader_program_load(&gui->poly_shader, "poly"))
		goto err_glew;

	if (!shader_program_load(&gui->tex_shader, "texu"))
		goto err_poly;

	if (!shader_program_load(&gui->txt_shader, "text"))
		goto err_texu;

	if (!font_install())
		goto err_text;

	gui->fonts = array_create();

	gui->creation_time = time_current();
	gui->frame_start_time = gui->creation_time;
	gui->frame_time_milli = 0;

	gui->repeat_delay = 500;
	gui->repeat_interval = 100;
	gui->repeat_timer = gui->repeat_delay;

	gui->default_style = g_default_style;
	gui_style_default(gui);

	gui->hot_id = gui->active_id = 0;
	gui->active_id_at_frame_start = 0;

	gui->drag_offset = g_v2i_zero;

	gui->pw_buf = array_create();

	goto out;

err_text:
	shader_program_destroy(&gui->txt_shader);
err_texu:
	shader_program_destroy(&gui->tex_shader);
err_poly:
	shader_program_destroy(&gui->poly_shader);
err_glew:
	SDL_GL_DeleteContext(gui->gl_context);
err_ctx:
	SDL_DestroyWindow(gui->window);
err_win:
	SDL_Quit();
err_sdl:
	free(gui);
	gui = NULL;
out:
	return gui;
}

void gui_destroy(gui_t *gui)
{
	array_destroy(gui->pw_buf);
	array_foreach(gui->fonts, font_t, f)
		font_destroy(f);
	array_destroy(gui->fonts);
	font_uninstall();
	shader_program_destroy(&gui->poly_shader);
	shader_program_destroy(&gui->tex_shader);
	shader_program_destroy(&gui->txt_shader);
	SDL_GL_DeleteContext(gui->gl_context);
	SDL_DestroyWindow(gui->window);
	SDL_Quit();
	free(gui);
}

void gui_dim(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->win_halfdim.x * 2;
	*y = gui->win_halfdim.y * 2;
}

void gui_minimize(gui_t *gui)
{
	SDL_MinimizeWindow(gui->window);
}

b32 gui_begin_frame(gui_t *gui)
{
	timepoint_t now = time_current();
	gui->frame_time_milli = time_diff_milli(gui->frame_start_time, now);
	gui->frame_start_time = now;

	b32 quit = false;
	SDL_Event evt;
	const u32 last_mouse_btn = gui->mouse_btn;
	gui->mouse_btn = 0;
	while (SDL_PollEvent(&evt) == 1) {
		switch (evt.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_MOUSEWHEEL:
			gui->mouse_btn |= (evt.wheel.y > 0 ? MB_WHEELUP : MB_WHEELDOWN);
			break;
		}
	}

	gui->mouse_btn |= SDL_GetMouseState(&gui->mouse_pos.x, &gui->mouse_pos.y);
	gui->mouse_btn_diff = gui->mouse_btn ^ last_mouse_btn;
	const v2i prev_win_halfdim = gui->win_halfdim;
	SDL_GetWindowSize(gui->window, &gui->win_halfdim.x, &gui->win_halfdim.y);
	gui->mouse_pos.y = gui->win_halfdim.y - gui->mouse_pos.y;
	static const v2i g_v2i_2 = { .x=2, .y=2 };
	v2i_div_eq(&gui->win_halfdim, g_v2i_2);
	if (!v2i_equal(prev_win_halfdim, gui->win_halfdim))
		glViewport(0, 0, 2*gui->win_halfdim.x, 2*gui->win_halfdim.y);

	gui->prev_key = gui->key;
	gui->key = 0;
	s32 key_cnt;
	const u8 *keys = SDL_GetKeyboardState(&key_cnt);
	const SDL_Keymod mod = SDL_GetModState();
	const b32 caps = ((mod & KMOD_SHIFT) != 0) != ((mod & KMOD_CAPS) != 0);
	for (s32 i = 0; i < key_cnt; ++i)
		if (keys[i] && gui__convert_scancode(i, caps, &gui->key))
			break;

	gui->active_id_at_frame_start = gui->active_id;

	float color[4];
	color_as_float_array(color, gui->style.bg_color);
	glClearColor(color[0], color[1], color[2], color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return !quit;
}

void gui_end_frame(gui_t *gui)
{
	glFlush();
	SDL_GL_SwapWindow(gui->window);
}

void gui_run(gui_t *gui, u32 fps, b32(*ufunc)(gui_t *gui, void *udata),
             void *udata)
{
	const u32 target_frame_milli = 1000/fps;
	timepoint_t start, end;
	b32 quit = false;
	while(gui_begin_frame(gui) && !quit)
	{
		start = time_current();
		quit = ufunc(gui, udata);
		gui_end_frame(gui);
		end = time_current();
		const u32 frame_milli = time_diff_milli(start, end);
		if (frame_milli < target_frame_milli)
			time_sleep_milli(target_frame_milli - frame_milli);
		else
			log_write("long frame: %ums", frame_milli);
	}
}


/* Input */

void mouse_pos(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->mouse_pos.x;
	*y = gui->mouse_pos.y;
}

b32 mouse_press(const gui_t *gui, u32 mask)
{
	return (gui->mouse_btn & mask) && (gui->mouse_btn_diff & mask);
}

b32 mouse_down(const gui_t *gui, u32 mask)
{
	return gui->mouse_btn & mask;
}

b32 mouse_release(const gui_t *gui, u32 mask)
{
	return !(gui->mouse_btn & mask) && (gui->mouse_btn_diff & mask);
}

b32 mouse_release_bg(const gui_t *gui, u32 mask)
{
	return    mouse_release(gui, mask)
	       && gui->active_id == 0
	       && gui->active_id_at_frame_start == 0;
}

void mouse_scroll(const gui_t *gui, s32 *dir)
{
	if (gui->mouse_btn & MB_WHEELUP)
		*dir = 1;
	else if (gui->mouse_btn & MB_WHEELDOWN)
		*dir = -1;
	else
		*dir = 0;
}

char key_press(const gui_t *gui)
{
	return gui->key != gui->prev_key ? gui->key : 0;
}

char key_down(const gui_t *gui)
{
	return gui->key;
}

char key_release(const gui_t *gui)
{
	return gui->key != gui->prev_key ? gui->prev_key : 0;
}


/* Retained Mode API */

static
void gui__set_color_attrib(shader_prog_t *p, color_t c)
{
	const GLint color_attrib = shader_program_uniform(p, "color");
	float color_f[4];
	color_as_float_array(color_f, c);
	glUniform4fv(color_attrib, 1, color_f);
}

static
void gui__set_win_halfdim_attrib(gui_t *gui, shader_prog_t *p)
{
	const GLint win_attrib = shader_program_uniform(p, "window_halfdim");
	glUniform2f(win_attrib, gui->win_halfdim.x, gui->win_halfdim.y);
}

void rmgui_poly_init(gui_t *gui, const v2f *v, u32 n, mesh_t *mesh, u32 *vao)
{
	mesh_init(mesh, v, n);

	glGenVertexArrays(1, vao);

	glBindVertexArray(*vao);
	mesh_bind(mesh);

	const GLint pos_attrib = shader_program_attrib(&gui->poly_shader, "position");
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pos_attrib);
}

void rmgui_line_init(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                     s32 w, color_t c, rmgui_poly_t *line)
{
	assert(w >= 1);
	if (w == 1) {
		v2f vertices[2] = {
			{ x0, y0 },
			{ x1, y1 }
		};
		rmgui_poly_init(gui, vertices, 2, &line->mesh, &line->vao);
		line->line_color = c;
		line->fill_color = g_nocolor;
	} else {
		v2f vertices[4] = {
			{ x0, y0 },
			{ x0, y0 },
			{ x1, y1 },
			{ x1, y1 }
		};
		v2f dir = v2f_scale(v2f_normalize(v2f_sub(vertices[2], vertices[0])), w/2.f);
		v2f perp = v2f_lperp(dir);

		vertices[0] = v2f_sub(v2f_sub(vertices[0], dir), perp);
		vertices[1] = v2f_add(v2f_sub(vertices[1], dir), perp);
		vertices[2] = v2f_add(v2f_add(vertices[2], dir), perp);
		vertices[3] = v2f_sub(v2f_add(vertices[3], dir), perp);

		rmgui_poly_init(gui, vertices, 4, &line->mesh, &line->vao);
		line->line_color = g_nocolor;
		line->fill_color = c;
	}
}

void rmgui_rect_init(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     color_t fill, color_t line, rmgui_poly_t *rect)
{
	box2f box;
	v2f poly[4];

	box2f_from_dims(&box, x, y + h, x + w, y);
	polyf_from_box(poly, box);

	rmgui_poly_init(gui, poly, 4, &rect->mesh, &rect->vao);
	rect->fill_color = fill;
	rect->line_color = line;
}

void rmgui_circ_init(gui_t *gui, s32 x, s32 y, s32 r, color_t fill,
                     color_t line, rmgui_poly_t *circ)
{
	v2f *poly = array_create();

	const u32 n = 4 + 2 * r;
	const r32 radians_slice = 2 * fPI / n;
	for (u32 i = 0; i < n; ++i) {
		const r32 radians = i * radians_slice;
		const v2f v = { .x=x+r*cos(radians), .y=y+r*sin(radians) };
		array_append(poly, v);
	}

	rmgui_poly_init(gui, poly, array_sz(poly), &circ->mesh, &circ->vao);
	circ->fill_color = fill;
	circ->line_color = line;

	array_destroy(poly);
}

void rmgui_poly_draw(gui_t *gui, const rmgui_poly_t *poly, s32 x, s32 y)
{
	GLint offset_attrib;

	glBindVertexArray(poly->vao);
	mesh_bind(&poly->mesh);
	shader_program_bind(&gui->poly_shader);

	gui__set_win_halfdim_attrib(gui, &gui->poly_shader);

	offset_attrib = shader_program_uniform(&gui->poly_shader, "offset");
	glUniform2f(offset_attrib, x, y);

	if (!color_equal(poly->fill_color, g_nocolor)) {
		gui__set_color_attrib(&gui->poly_shader, poly->fill_color);
		glDrawArrays(GL_TRIANGLE_FAN, 0, poly->mesh.sz);
	}

	if (!color_equal(poly->line_color, g_nocolor)) {
		gui__set_color_attrib(&gui->poly_shader, poly->line_color);
		glDrawArrays(GL_LINE_LOOP, 0, poly->mesh.sz);
	}
}

void rmgui_poly_destroy(rmgui_poly_t *poly)
{
	glBindVertexArray(0);
	mesh_unbind();
	glDeleteVertexArrays(1, &poly->vao);
	mesh_destroy(&poly->mesh);
}

void rmgui_img_draw(gui_t *gui, const img_t *img, s32 x, s32 y)
{
	rmgui_img_draw_scaled(gui, img, x, y, 1.f, 1.f);
}

void rmgui_img_draw_scaled(gui_t *gui, const img_t *img, s32 x, s32 y,
                           r32 sx, r32 sy)
{
	shader_program_bind(&gui->tex_shader);
	gui__set_win_halfdim_attrib(gui, &gui->tex_shader);
	img_render(img, x, y, sx, sy, &gui->tex_shader);
	shader_program_unbind();
}


/* Immediate Mode API */

void gui_line(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w, color_t c)
{
	rmgui_poly_t line;
	rmgui_line_init(gui, x0, y0, x1, y1, w, c, &line);
	rmgui_poly_draw(gui, &line, 0, 0);
	rmgui_poly_destroy(&line);
}

void gui_rect(gui_t *gui, s32 x, s32 y, s32 w, s32 h, color_t fill, color_t line)
{
	rmgui_poly_t rect;
	rmgui_rect_init(gui, x, y, w, h, fill, line, &rect);
	rmgui_poly_draw(gui, &rect, 0, 0);
	rmgui_poly_destroy(&rect);
}

void gui_circ(gui_t *gui, s32 x, s32 y, s32 r, color_t fill, color_t line)
{
	rmgui_poly_t circ;
	rmgui_circ_init(gui, x, y, r, fill, line, &circ);
	rmgui_poly_draw(gui, &circ, 0, 0);
	rmgui_poly_destroy(&circ);
}

void gui_poly(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t line)
{
	rmgui_poly_t poly;
	v2f **polys;
	if (polyf_is_convex(v, n) || fill.a == 0) {
		poly.fill_color = fill;
		poly.line_color = line;
		rmgui_poly_init(gui, v, n, &poly.mesh, &poly.vao);
		rmgui_poly_draw(gui, &poly, 0, 0);
		rmgui_poly_destroy(&poly);
	} else {
		polys = array_create();
		polyf_decompose(v, n, &polys);
		array_foreach(polys, v2f*, poly) {
			gui_poly(gui, *poly, array_sz(*poly), fill, g_nocolor);
			array_destroy(*poly);
		}
		array_destroy(polys);

		if (line.a != 0)
			gui_poly(gui, v, n, g_nocolor, line);
	}
}

void gui_img(gui_t *gui, s32 x, s32 y, const char *filename)
{
	img_t img;
	if (!img_load(&img, filename))
		return;
	rmgui_img_draw(gui, &img, x, y);
	img_destroy(&img);
}

static inline
font_t *gui__get_font(gui_t *gui, u32 sz)
{
	font_t *font = gui->fonts, *font_end = array_end(gui->fonts);
	while (font != font_end && font->sz != sz)
		++font;
	if (font != font_end)
		return font;

	font = array_append_null(gui->fonts);
	if (font_load(font, "Roboto.ttf", sz)) {
		return font;
	} else {
		array_pop(gui->fonts);
		return NULL;
	}
}

void gui__txt(gui_t *gui, s32 *x, s32 *y, s32 sz, const char *txt,
              color_t c, font_align_t align)
{
	shader_program_bind(&gui->txt_shader);

	gui__set_color_attrib(&gui->txt_shader, c);
	gui__set_win_halfdim_attrib(gui, &gui->txt_shader);

	font_t *font = gui__get_font(gui, sz);
	assert(font);
	font_render_ex(font, txt, x, y, &gui->txt_shader, align);

	shader_program_unbind();
}

void gui_txt(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt,
             color_t c, font_align_t align)
{
	gui__txt(gui, &x, &y, sz, txt, c, align);
}

void gui_mask(gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	glScissor(x, y, w, h);
}

void gui_unmask(gui_t *gui)
{
	glScissor(0, 0, gui->win_halfdim.x * 2, gui->win_halfdim.y * 2);
}


/* Widgets */

static
b32 gui__allow_new_interaction(const gui_t *gui)
{
	return !mouse_down(gui, MB_LEFT | MB_MIDDLE | MB_RIGHT);
}

typedef enum widget_style_t
{
	INACTIVE,
	HOT,
	ACTIVE
} widget_style_t;

static
void widget__color(const gui_t *gui, u64 id,
                   color_t *fill, color_t *outline, color_t *text)
{
	if (gui->active_id == id) {
		if (fill)
			*fill = gui->style.active_color;
		if (outline)
			*outline = gui->style.active_line_color;
		if (text)
			*text = gui->style.active_text_color;
	} else if (gui->hot_id == id) {
		if (fill)
			*fill = gui->style.hot_color;
		if (outline)
			*outline = gui->style.hot_line_color;
		if (text)
			*text = gui->style.hot_text_color;
	} else {
		if (fill)
			*fill = gui->style.fill_color;
		if (outline)
			*outline = gui->style.outline_color;
		if (text)
			*text = gui->style.text_color;
	}
}

static
b32 gui__can_repeat(gui_t *gui)
{
	if (gui->repeat_timer <= gui->frame_time_milli) {
		gui->repeat_timer =   gui->repeat_interval
		                    - (gui->frame_time_milli - gui->repeat_timer);
		return true;
	} else {
		gui->repeat_timer -= gui->frame_time_milli;
		return false;
	}
}

b32 gui_npt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
            font_align_t align, npt_flags_t flags)
{
	const u64 id = (u64)txt;
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	const b32 contains_mouse = box2i_contains_point(box, gui->mouse_pos);
	b32 complete = false;
	if (gui->active_id == id) {
		if (gui->key != 0) {
			b32 modify = false;
			if (gui->key != gui->prev_key) {
				modify = true;
				gui->repeat_timer = gui->repeat_delay;
			} else if (gui__can_repeat(gui)) {
				modify = true;
			}
			if (modify) {
				const u32 len = strlen(txt);
				if (gui->key == KEY_BACKSPACE) {
					if (len > 0)
						txt[len-1] = '\0';
				} else if (gui->key == KEY_RETURN) {
					gui->active_id = 0;
					complete = true;
				} else if (   gui->key >= 32
				           && ((flags & NPT_NUMERIC) == 0 || isdigit(gui->key))) {
					char buf[2] = { gui->key, '\0' };
					strncat(txt, buf, n-len-1);
				}
			}
		} else {
			gui->repeat_timer = gui->repeat_delay;
		}
		if (mouse_press(gui, MB_LEFT) && !contains_mouse)
			gui->active_id = 0;
	} else if (gui->hot_id == id) {
		if (!contains_mouse) {
			gui->hot_id = 0;
		} else if (mouse_press(gui, MB_LEFT)) {
			gui->active_id = id;
			gui->hot_id = 0;
		}
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = id;
	}


	color_t fill, line, text_color;
	widget__color(gui, id, &fill, &line, &text_color);
	gui_rect(gui, x, y, w, h, fill, line);
	switch (align) {
	case FONT_ALIGN_LEFT:
		x += 2;
	break;
	case FONT_ALIGN_CENTER:
		x = x+w/2;
	break;
	case FONT_ALIGN_RIGHT:
		x = x+w-2;
	break;
	}
	s32 txt_y = y + h*(1.f-gui->style.font_ratio)/2.f;
	if (flags & NPT_PASSWORD) {
		const u32 sz = strlen(txt);
		array_reserve(gui->pw_buf, sz+1);
		for (u32 i = 0; i < sz; ++i)
			gui->pw_buf[i] = '*';
		gui->pw_buf[sz] = '\0';
		gui__txt(gui, &x, &txt_y, h*gui->style.font_ratio, gui->pw_buf,
		         text_color, align);
	} else {
		gui__txt(gui, &x, &txt_y, h*gui->style.font_ratio, txt,
		         text_color, align);
	}
	if (gui->active_id == id) {
		const u32 milli_since_creation =
			time_diff_milli(gui->creation_time, gui->frame_start_time);
		if (milli_since_creation % 1000 < 500)
			gui_line(gui, x+1, txt_y, x+1, y+h-2, 1, text_color);
	}
	return complete;
}

static
btn_val_t gui__btn_logic(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                         const char *txt, u64 id, b32 *contains_mouse)
{
	btn_val_t retval = BTN_NONE;
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	*contains_mouse = box2i_contains_point(box, gui->mouse_pos);
	if (gui->hot_id == id) {
		if (!*contains_mouse) {
			gui->hot_id = 0;
		} else if (mouse_press(gui, MB_LEFT)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->repeat_timer = gui->repeat_delay;
		}
	} else if (gui->active_id == id) {
		if (mouse_release(gui, MB_LEFT)) {
			if (*contains_mouse)
				retval = BTN_PRESS;
			gui->active_id = 0;
			gui->repeat_timer = gui->repeat_delay;
		} else if (!*contains_mouse) {
			gui->repeat_timer = gui->repeat_delay;
		} else if (gui__can_repeat(gui)) {
			retval = BTN_HOLD;
		}
	} else if (gui__allow_new_interaction(gui) && *contains_mouse) {
		gui->hot_id = id;
	}
	return retval;
}

static
void gui__btn_render(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                         const char *txt, color_t fill,
                         color_t text_color)
{
	gui_rect(gui, x, y, w, h, fill, gui->style.outline_color);
	const s32 txt_y = y + h*(1.f-gui->style.font_ratio)/2.f;
	gui_txt(gui, x+w/2, txt_y, h*gui->style.font_ratio, txt,
	        text_color, FONT_ALIGN_CENTER);
}

btn_val_t gui_btn(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt)
{
	const u64 _x = x, _y = y;
	const u64 id = (_x << 48) | (_y << 32) | hash(txt);
	b32 contains_mouse;
	const btn_val_t ret = gui__btn_logic(gui, x, y, w, h, txt, id, &contains_mouse);

	color_t c = gui->style.fill_color;
	color_t text_color = gui->style.text_color;
	if (gui->active_id == id) {
		if (contains_mouse) {
			c = gui->style.active_color;
			text_color = gui->style.active_text_color;
		} else {
			c = gui->style.hot_color;
			text_color = gui->style.hot_text_color;
		}
	} else if (gui->hot_id == id) {
		c = gui->style.hot_color;
		text_color = gui->style.hot_text_color;
	}
	gui__btn_render(gui, x, y, w, h, txt, c, text_color);
	return ret;
}

void gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, b32 *val)
{
	const u64 _x = x, _y = y;
	const u64 id = (_x << 48) | (_y << 32) | hash(txt);
	b32 contains_mouse;
	const b32 was_checked = *val;
	if (gui__btn_logic(gui, x, y, w, h, txt, id, &contains_mouse) == BTN_PRESS)
		*val = !*val;

	color_t c = gui->style.fill_color;
	color_t text_color = gui->style.text_color;
	if (gui->active_id == id || (*val && !was_checked)) {
		c = gui->style.active_color;
		text_color = gui->style.active_text_color;
	}
	if (gui->hot_id == id && contains_mouse) {
		c = gui->style.hot_color;
		text_color = gui->style.hot_text_color;
	} else if (*val) {
		c = gui->style.active_color;
		text_color = gui->style.active_text_color;
	}
	gui__btn_render(gui, x, y, w, h, txt, c, text_color);
}

static
void gui__slider(gui_t *gui, s32 x, s32 y, s32 w, s32 h, s32 hnd_len, r32 *val)
{
	assert(*val >= 0.f && *val <= 1.f);
	assert(w != h);

	const u64 id = (u64)val;
	const b32 vert = w < h;
	box2i box;
	if (vert) {
		box.min = (v2i){ .x=x, .y=y+(h-hnd_len)**val };
		box.max = (v2i){ .x=x+w, .y=y+hnd_len+(h-hnd_len)**val };
	} else {
		box.min = (v2i){ .x=x+(w-hnd_len)**val, .y=y };
		box.max = (v2i){ .x=x+hnd_len+(w-hnd_len)**val, .y=y+h };
	}
	const b32 contains_mouse = box2i_contains_point(box, gui->mouse_pos);

	if (gui->hot_id == id) {
		if (!contains_mouse) {
			gui->hot_id = 0;
		} else if (mouse_press(gui, MB_LEFT)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->drag_offset.x = box.min.x+(box.max.x-box.min.x)/2-gui->mouse_pos.x;
			gui->drag_offset.y = box.min.y+(box.max.y-box.min.y)/2-gui->mouse_pos.y;
		}
	} else if (gui->active_id == id) {
		if (!mouse_release(gui, MB_LEFT)) {
			if (vert) {
				const r32 min_y = y+hnd_len/2;
				const r32 max_y = y+h-hnd_len/2;
				const s32 mouse_y = gui->mouse_pos.y+gui->drag_offset.y;
				*val = fmath_clamp(0, (mouse_y-min_y)/(max_y-min_y), 1.f);
			} else {
				const r32 min_x = x+hnd_len/2;
				const r32 max_x = x+w-hnd_len/2;
				const s32 mouse_x = gui->mouse_pos.x+gui->drag_offset.x;
				*val = fmath_clamp(0, (mouse_x-min_x)/(max_x-min_x), 1.f);
			}
		} else {
			gui->active_id = 0;
		}
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = id;
	}

	const color_t outline = gui->style.outline_color;
	if (vert) {
		gui_line(gui, x+w/2, y+hnd_len/2, x+w/2, y+h-hnd_len/2, 1, outline);
	} else {
		gui_line(gui, x+hnd_len/2, y+h/2, x+w-hnd_len/2, y+h/2, 1, outline);
	}
	color_t c = gui->style.fill_color;
	if (gui->active_id == id)
		c = gui->style.active_color;
	else if (gui->hot_id == id)
		c = gui->style.hot_color;
	if (vert)
		gui_rect(gui, x, y+(h-hnd_len)**val, w, hnd_len, c, outline);
	else
		gui_rect(gui, x+(w-hnd_len)**val, y, hnd_len, h, c, outline);
}

void gui_slider(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val)
{
	const s32 hnd_len = min(w, h);
	gui__slider(gui, x, y, w, h, hnd_len, val);
}

void gui_select(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                const char *txt, u32 *val, u32 opt)
{
	const b32 selected = *val == opt;
	const u64 id = (u64)val + opt;
	b32 contains_mouse;
	if (   gui__btn_logic(gui, x, y, w, h, txt, id,
	                      &contains_mouse) == BTN_PRESS
	    && !selected) {
		*val = opt;
	}

	color_t c = gui->style.fill_color;
	color_t text_color = gui->style.text_color;
	if (gui->active_id == id || *val == opt) {
		c = gui->style.active_color;
		text_color = gui->style.active_text_color;
	}
	if (gui->hot_id == id && contains_mouse) {
		c = gui->style.hot_color;
		text_color = gui->style.hot_text_color;
	}
	gui__btn_render(gui, x, y, w, h, txt, c, text_color);
}

void gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                 const char *txt, u32 *val, u32 opt)
{
	const u64 id = (u64)val + opt;
	b32 contains_mouse;
	if (gui__btn_logic(gui, x, y, w, h, txt, id, &contains_mouse) == BTN_PRESS) {
	  if (!(*val & opt))
			*val |= opt;
		else if (*val & ~opt)
			*val &= ~opt;
	}

	color_t c = gui->style.fill_color;
	color_t text_color = gui->style.text_color;
	if (gui->active_id == id || (*val & opt)) {
		c = gui->style.active_color;
		text_color = gui->style.active_text_color;
	}
	if (gui->hot_id == id && contains_mouse) {
		c = gui->style.hot_color;
		text_color = gui->style.hot_text_color;
	}
	gui__btn_render(gui, x, y, w, h, txt, c, text_color);
}

static
b32 gui__drag(gui_t *gui, s32 *x, s32 *y, b32 contains_mouse,
              mouse_button_t mb, u64 *_id)
{
	b32 retval = false;
	const u64 id = (u64)x;
	*_id = id;
	if (gui->hot_id == id) {
		if (!contains_mouse) {
			gui->hot_id = 0;
		} else if (mouse_press(gui, mb)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->drag_offset.x = *x - gui->mouse_pos.x;
			gui->drag_offset.y = *y - gui->mouse_pos.y;
		}
	} else if (gui->active_id == id) {
		*x = gui->mouse_pos.x + gui->drag_offset.x;
		*y = gui->mouse_pos.y + gui->drag_offset.y;
		retval = true;
		if (mouse_release(gui, mb))
			gui->active_id = 0;
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = id;
	}
	return retval;
}

b32 gui_drag(gui_t *gui, s32 *x, s32 *y, u32 w, u32 h, mouse_button_t mb)
{
	box2i box;
	box2i_from_dims(&box, *x, *y+h, *x+w, *y);
	const b32 contains_mouse = box2i_contains_point(box, gui->mouse_pos);
	u64 id;
	const b32 ret = gui__drag(gui, x, y, contains_mouse, mb, &id);
	color_t fill, outline;
	widget__color(gui, id, &fill, &outline, NULL);
	gui_rect(gui, *x, *y, w, h, fill, outline);
	return ret;
}

b32 gui_cdrag(gui_t *gui, s32 *x, s32 *y, u32 r, mouse_button_t mb)
{
	const v2i pos = { .x=*x, .y=*y };
	const b32 contains_mouse = v2i_dist_sq(pos, gui->mouse_pos) <= r*r;
	u64 id;
	const b32 ret = gui__drag(gui, x, y, contains_mouse, mb, &id);
	color_t fill, outline;
	widget__color(gui, id, &fill, &outline, NULL);
	gui_circ(gui, *x, *y, r, fill, outline);
	return ret;
}

void gui_scrollbar(gui_t *gui, s32 x, s32 y, s32 w, s32 h, s32 ttl_dim, r32 *off)
{
	assert(w != h);
	assert(ttl_dim > max(w, h));

	if (w < h)
		*off = 1.f - *off;

	const s32 miss = ttl_dim-max(w, h);
	const s32 hnd_len = max(min(w, h), max(w, h)-miss);
	gui__slider(gui, x, y, w, h, hnd_len, off);

	if (w < h)
		*off = 1.f - *off;
}


/* Style */

const gui_style_t *gui_get_style(gui_t *gui)
{
	return &gui->style;
}

void gui_style(gui_t *gui, const gui_style_t *style)
{
	gui->style = *style;
}

void gui_style_default(gui_t *gui)
{
	gui_style(gui, &gui->default_style);
}

#undef GUI_IMPLEMENTATION
#endif // GUI_IMPLEMENTATION
