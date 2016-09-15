#include <assert.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL.h>

#include "violet/gui/font.h"
#include "violet/gui/gui.h"
#include "violet/gui/img.h"
#include "violet/gui/mesh.h"
#include "violet/gui/shader.h"
#include "violet/gui/texture.h"
#include "violet/math/decompose.h"
#include "violet/math/r32.h"
#include "violet/math/s32.h"
#include "violet/structures/array_map.h"
#include "violet/utility/hash.h"
#include "violet/utility/log.h"
#include "violet/utility/time.h"


const vlt_style g_default_style = {
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


int SDL_FilterEvent(void *userdata, SDL_Event *event)
{
	switch (event->type)
	{
	case SDL_QUIT:
	case SDL_MOUSEWHEEL:
		return true;
	default:
		return false;
	}
}

typedef enum key
{
	KEY_RETURN = 13,
	KEY_BACKSPACE = 8
} key;

b8 _convert_scancode(SDL_Scancode code, char *c)
{
	const SDL_Keycode key = SDL_GetKeyFromScancode(code);
	if (key >= 0 && key <= 255)
		*c = key;
	else if (key >= SDLK_KP_DIVIDE && key <= SDLK_KP_PERIOD)
	{
		static char keys[1 + SDLK_KP_PERIOD - SDLK_KP_DIVIDE] = {
			'/', '*', '-', '+', KEY_RETURN, '1', '2', '3', '4', '5', '6',
			'7', '8', '9', '0', '.'
		};
		*c = keys[key - SDLK_KP_DIVIDE];
	}
	else
		return false;

	return true;
}

typedef struct vlt_gui
{
	vlt_time creation_time;
	vlt_time frame_start_time;
	u32 frame_time_milli;
	SDL_Window *window;
	SDL_GLContext gl_context;
	vlt_shader_program poly_shader;
	vlt_shader_program tex_shader;
	vlt_shader_program txt_shader;
	v2i win_halfdim;
	v2i mouse_pos;
	u32 mouse_btn;
	u32 mouse_btn_diff;
	char prev_key, key;
	u32 repeat_delay;
	u32 repeat_interval;
	u32 repeat_timer;
	array_map fonts; // sz -> vlt_font*
	vlt_style default_style;
	vlt_style style;
	u64 hot_id;
	u64 active_id;
	v2i drag_offset;
	array pw_buf;
} vlt_gui;

vlt_gui *vlt_gui_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                        vlt_gui_flags flags)
{
	vlt_gui *gui = calloc(1, sizeof(vlt_gui));
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		log_write("SDL_Init(VIDEO) failed: %s", SDL_GetError());
		goto err_sdl;
	}

	// Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_GetNumVideoDisplays() < 1)
	{
		log_write("could not create window: no video displays found");
		goto err_win;
	}

	SDL_DisplayMode display_mode;
	if (SDL_GetCurrentDisplayMode(0, &display_mode) != 0)
	{
		log_write("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
		goto err_win;
	}

	u32 sdl_flags = SDL_WINDOW_OPENGL;
	s32 min_h = display_mode.h - 60;
	if (flags & VLT_WINDOW_BORDERLESS)
	{
		sdl_flags |= SDL_WINDOW_BORDERLESS;
		min_h += 30;
	}
	if (flags & VLT_WINDOW_RESIZABLE)
		sdl_flags |= SDL_WINDOW_RESIZABLE;
#ifdef _WIN32
	gui->window = SDL_CreateWindow(title, x, y,
		min(w, display_mode.w), min(h, min_h), sdl_flags);
#else
	gui->window = SDL_CreateWindow(title, x, max(y, 30),
		min(w, display_mode.w), min(h, min_h), sdl_flags);
#endif
	if (gui->window == NULL)
	{
		log_write("SDL_CreateWindow failed: %s", SDL_GetError());
		goto err_win;
	}

	gui->gl_context = SDL_GL_CreateContext(gui->window);
	if (gui->gl_context == NULL)
	{
		log_write("SDL_CreateContext failed: %s", SDL_GetError());
		goto err_ctx;
	}

	SDL_SetEventFilter(SDL_FilterEvent, NULL);

	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK)
	{
		log_write("glewInit error: %s", glewGetErrorString(glew_err));
		goto err_glew;
	}

	log_write("GL version: %s", glGetString(GL_VERSION));

	glEnable(GL_MULTISAMPLE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	if (!vlt_shader_program_load(&gui->poly_shader, "poly"))
		goto err_glew;

	if (!vlt_shader_program_load(&gui->tex_shader, "texu"))
		goto err_poly;

	if (!vlt_shader_program_load(&gui->txt_shader, "text"))
		goto err_texu;

	if (!vlt_font_install())
		goto err_text;

	array_map_init(&gui->fonts, sizeof(u32), sizeof(vlt_font*));

	vlt_get_time(&gui->creation_time);
	gui->frame_start_time = gui->creation_time;
	gui->frame_time_milli = 0;

	gui->repeat_delay = 500;
	gui->repeat_interval = 100;
	gui->repeat_timer = gui->repeat_delay;

	gui->default_style = g_default_style;
	vlt_gui_style_default(gui);

	gui->hot_id = gui->active_id = 0;

	gui->drag_offset = g_v2i_zero;

	array_init(&gui->pw_buf, sizeof(char));

	goto out;

err_text:
	vlt_shader_program_destroy(&gui->txt_shader);
err_texu:
	vlt_shader_program_destroy(&gui->tex_shader);
err_poly:
	vlt_shader_program_destroy(&gui->poly_shader);
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

static void _font_clear(void *f)
{
	vlt_font *font = *(vlt_font**)f;
	vlt_font_destroy(font);
	vlt_font_free(font);
}

void vlt_gui_destroy_window(vlt_gui *gui)
{
	array_destroy(&gui->pw_buf);
	array_map_destroy_deep(&gui->fonts, _font_clear);
	vlt_font_uninstall();
	vlt_shader_program_destroy(&gui->poly_shader);
	vlt_shader_program_destroy(&gui->tex_shader);
	vlt_shader_program_destroy(&gui->txt_shader);
	SDL_GL_DeleteContext(gui->gl_context);
	SDL_DestroyWindow(gui->window);
	SDL_Quit();
}

void vlt_gui_destroy(vlt_gui *gui)
{
	free(gui);
}

void vlt_gui_dim(const vlt_gui *gui, s32 *x, s32 *y)
{
	*x = gui->win_halfdim.x * 2;
	*y = gui->win_halfdim.y * 2;
}

void vlt_gui_minimize(vlt_gui *gui)
{
	SDL_MinimizeWindow(gui->window);
}

void vlt_gui_mouse_pos(const vlt_gui *gui, s32 *x, s32 *y)
{
	*x = gui->mouse_pos.x;
	*y = gui->mouse_pos.y;
}

b8 vlt_gui_mouse_press(const vlt_gui *gui, u32 mask)
{
	return (gui->mouse_btn & mask) && (gui->mouse_btn_diff & mask);
}

b8 vlt_gui_mouse_down(const vlt_gui *gui, u32 mask)
{
	return gui->mouse_btn & mask;
}

b8 vlt_gui_mouse_release(const vlt_gui *gui, u32 mask)
{
	return !(gui->mouse_btn & mask) && (gui->mouse_btn_diff & mask);
}

b8 vlt_gui_mouse_release_bg(const vlt_gui *gui, u32 mask)
{
	return vlt_gui_mouse_release(gui, mask)
		&& gui->active_id == 0;
}

void vlt_gui_mouse_scroll(const vlt_gui *gui, s32 *dir)
{
	if (gui->mouse_btn & VLT_MB_WHEELUP)
		*dir = 1;
	else if (gui->mouse_btn & VLT_MB_WHEELDOWN)
		*dir = -1;
	else
		*dir = 0;
}

char vlt_gui_key_press(const vlt_gui *gui)
{
	return gui->key != gui->prev_key ? gui->key : 0;
}

char vlt_gui_key_down(const vlt_gui *gui)
{
	return gui->key;
}

char vlt_gui_key_release(const vlt_gui *gui)
{
	return gui->key != gui->prev_key ? gui->prev_key : 0;
}

b8 vlt_gui_begin_frame(vlt_gui *gui)
{
	vlt_time now;
	vlt_get_time(&now);
	gui->frame_time_milli = vlt_diff_milli(&gui->frame_start_time, &now);
	gui->frame_start_time = now;

	b8 quit = false;
	SDL_Event evt;
	const u32 last_mouse_btn = gui->mouse_btn;
	gui->mouse_btn = 0;
	while (SDL_PollEvent(&evt) == 1)
	{
		switch (evt.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_MOUSEWHEEL:
			gui->mouse_btn |= (evt.wheel.y > 0 ?
				VLT_MB_WHEELUP :
				VLT_MB_WHEELDOWN);
			break;
		}
	}

	gui->mouse_btn |= SDL_GetMouseState(&gui->mouse_pos.x,
		&gui->mouse_pos.y);
	gui->mouse_btn_diff = gui->mouse_btn ^ last_mouse_btn;
	const v2i prev_win_halfdim = gui->win_halfdim;
	SDL_GetWindowSize(gui->window, &gui->win_halfdim.x,
		&gui->win_halfdim.y);
	gui->mouse_pos.y = gui->win_halfdim.y - gui->mouse_pos.y;
	static const v2i g_v2i_2 = { .x=2, .y=2 };
	v2i_div(&gui->win_halfdim, &g_v2i_2, &gui->win_halfdim);
	if (!v2i_equal(&prev_win_halfdim, &gui->win_halfdim))
		glViewport(0, 0, 2*gui->win_halfdim.x, 2*gui->win_halfdim.y);

	gui->prev_key = gui->key;
	gui->key = 0;
	s32 key_cnt;
	const u8 *keys = SDL_GetKeyboardState(&key_cnt);
	for (s32 i = 0; i < key_cnt; ++i)
		if (keys[i] && _convert_scancode(i, &gui->key))
			break;

	float color[4];
	vlt_color_as_float_array(color, gui->style.bg_color);
	glClearColor(color[0], color[1], color[2], color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return !quit;
}

void vlt_gui_end_frame(vlt_gui *gui)
{
	glFlush();
	SDL_GL_SwapWindow(gui->window);
}


/* Retained Mode API */

static void _set_color_attrib(vlt_shader_program *p, vlt_color c)
{
	const GLint color_attrib = vlt_shader_program_uniform(p, "color");
	float color_f[4];
	vlt_color_as_float_array(color_f, c);
	glUniform4fv(color_attrib, 1, color_f);
}

static void _set_win_halfdim_attrib(vlt_gui *gui, vlt_shader_program *p)
{
	const GLint win_attrib = vlt_shader_program_uniform(p,
		"window_halfdim");
	glUniform2f(win_attrib, gui->win_halfdim.x, gui->win_halfdim.y);
}

void vlt_rmgui_poly_init(vlt_gui *gui, const v2f *v, u32 n,
                         vlt_mesh *mesh, u32 *vao)
{
	vlt_mesh_init(mesh, v, n);

	glGenVertexArrays(1, vao);

	glBindVertexArray(*vao);
	vlt_mesh_bind(mesh);

	const GLint pos_attrib =
		vlt_shader_program_attrib(&gui->poly_shader, "position");
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pos_attrib);
}

void vlt_rmgui_line_init(vlt_gui *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                         s32 w, vlt_color c, vlt_rmgui_poly *line)
{
	assert(w >= 1);
	if (w == 1)
	{
		v2f vertices[2] = {
			{ x0, y0 },
			{ x1, y1 }
		};
		vlt_rmgui_poly_init(gui, vertices, 2, &line->mesh, &line->vao);
		line->line_color = c;
		line->fill_color = g_nocolor;
	}
	else
	{
		v2f vertices[4] = {
			{ x0, y0 },
			{ x0, y0 },
			{ x1, y1 },
			{ x1, y1 }
		};
		v2f dir, perp;
		v2f_sub(vertices+2, vertices, &dir);
		v2f_normalize(&dir, &dir);
		v2f_scale(&dir, w/2.f, w/2.f, &dir);
		v2f_perp(&dir, true, &perp);
		v2f_sub(vertices, &dir, vertices);
		v2f_sub(vertices, &perp, vertices);
		v2f_sub(vertices+1, &dir, vertices+1);
		v2f_add(vertices+1, &perp, vertices+1);
		v2f_add(vertices+2, &dir, vertices+2);
		v2f_add(vertices+2, &perp, vertices+2);
		v2f_add(vertices+3, &dir, vertices+3);
		v2f_sub(vertices+3, &perp, vertices+3);

		vlt_rmgui_poly_init(gui, vertices, 4, &line->mesh, &line->vao);
		line->line_color = g_nocolor;
		line->fill_color = c;
	}
}

void vlt_rmgui_rect_init(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                         vlt_color fill, vlt_color line, vlt_rmgui_poly *rect)
{
	box2f box;
	box2f_from_dims(&box, x, y + h, x + w, y);
	v2f poly[4];
	polyf_from_box(poly, &box);

	vlt_rmgui_poly_init(gui, poly, 4, &rect->mesh, &rect->vao);
	rect->fill_color = fill;
	rect->line_color = line;
}

void vlt_rmgui_circ_init(vlt_gui *gui, s32 x, s32 y, s32 r, vlt_color fill,
                         vlt_color line, vlt_rmgui_poly *circ)
{
	array poly;
	array_init(&poly, sizeof(v2f));

	const u32 n = 4 + 2 *r;
	const r32 radians_slice = 2 *PI / n;
	for (u32 i = 0; i < n; ++i)
	{
		const r32 radians = i *radians_slice;
		const v2f v = { .x=x+r*cos(radians), .y=y+r*sin(radians) };
		array_append(&poly, &v);
	}

	vlt_rmgui_poly_init(gui, poly.data, poly.size, &circ->mesh, &circ->vao);
	circ->fill_color = fill;
	circ->line_color = line;

	array_destroy(&poly);
}

void vlt_rmgui_poly_draw(vlt_gui *gui, const vlt_rmgui_poly *poly, s32 x, s32 y)
{
	glBindVertexArray(poly->vao);
	vlt_mesh_bind(&poly->mesh);
	vlt_shader_program_bind(&gui->poly_shader);

	_set_win_halfdim_attrib(gui, &gui->poly_shader);

	const GLint offset_attrib =
		vlt_shader_program_uniform(&gui->poly_shader, "offset");
	glUniform2f(offset_attrib, x, y);

	if (!vlt_color_equal(poly->fill_color, g_nocolor))
	{
		_set_color_attrib(&gui->poly_shader, poly->fill_color);
		glDrawArrays(GL_TRIANGLE_FAN, 0, poly->mesh.sz);
	}

	if (!vlt_color_equal(poly->line_color, g_nocolor))
	{
		_set_color_attrib(&gui->poly_shader, poly->line_color);
		glDrawArrays(GL_LINE_LOOP, 0, poly->mesh.sz);
	}
}

void vlt_rmgui_poly_destroy(vlt_rmgui_poly *poly)
{
	glBindVertexArray(0);
	vlt_mesh_unbind(&poly->mesh);

	glDeleteVertexArrays(1, &poly->vao);

	vlt_mesh_destroy(&poly->mesh);
}

void vlt_rmgui_img_draw(vlt_gui *gui, vlt_img *img, s32 x, s32 y)
{
	vlt_shader_program_bind(&gui->tex_shader);

	_set_win_halfdim_attrib(gui, &gui->tex_shader);

	vlt_img_render(img, x, y, &gui->tex_shader);

	vlt_shader_program_unbind();
}


/* Immediate Mode API */

void vlt_gui_line(vlt_gui *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w,
                  vlt_color c)
{
	vlt_rmgui_poly line;
	vlt_rmgui_line_init(gui, x0, y0, x1, y1, w, c, &line);
	vlt_rmgui_poly_draw(gui, &line, 0, 0);
	vlt_rmgui_poly_destroy(&line);
}

void vlt_gui_rect(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                  vlt_color fill, vlt_color line)
{
	vlt_rmgui_poly rect;
	vlt_rmgui_rect_init(gui, x, y, w, h, fill, line, &rect);
	vlt_rmgui_poly_draw(gui, &rect, 0, 0);
	vlt_rmgui_poly_destroy(&rect);
}

void vlt_gui_circ(vlt_gui *gui, s32 x, s32 y, s32 r,
                  vlt_color fill, vlt_color line)
{
	vlt_rmgui_poly circ;
	vlt_rmgui_circ_init(gui, x, y, r, fill, line, &circ);
	vlt_rmgui_poly_draw(gui, &circ, 0, 0);
	vlt_rmgui_poly_destroy(&circ);
}

void vlt_gui_poly(vlt_gui *gui, const v2f *v, u32 n,
                  vlt_color fill, vlt_color line)
{
	if (polyf_is_convex(v, n) || fill.a == 0)
	{
		vlt_rmgui_poly poly = { .fill_color = fill, .line_color = line };
		vlt_rmgui_poly_init(gui, v, n, &poly.mesh, &poly.vao);
		vlt_rmgui_poly_draw(gui, &poly, 0, 0);
		vlt_rmgui_poly_destroy(&poly);
	}
	else
	{
		array polys;
		array_init(&polys, sizeof(array));
		polyf_decompose(v, n, &polys);
		for (u32 i = 0; i < polys.size; ++i)
		{
			array *poly = array_get(&polys, i);
			vlt_gui_poly(gui, poly->data, poly->size, fill, g_nocolor);
			array_destroy(poly);
		}
		array_destroy(&polys);

		if (line.a != 0)
			vlt_gui_poly(gui, v, n, g_nocolor, line);
	}
}

void vlt_gui_img(vlt_gui *gui, s32 x, s32 y, const char *filename)
{
	vlt_img *img = vlt_img_create();
	if (!vlt_img_load(img, filename))
		return;

	vlt_rmgui_img_draw(gui, img, x, y);

	vlt_img_destroy(img);
	vlt_img_free(img);
}

static vlt_font *_get_font(vlt_gui *gui, u32 sz)
{
	vlt_font ** f = array_map_get(&gui->fonts, &sz);
	if (f)
		return *f;

	vlt_font *font = vlt_font_create();
#ifdef __unix__
	if (vlt_font_load(font, "UbuntuMono.ttf", sz))
#else
	if (vlt_font_load(font, "MyriadProRegular.ttf", sz))
#endif
	{
		array_map_insert(&gui->fonts, &sz, &font);
		return font;
	}
	else
	{
		vlt_font_free(font);
		return NULL;
	}
}

void _vlt_gui_txt(vlt_gui *gui, s32 *x, s32 *y, s32 sz, const char *txt,
                  vlt_color c, font_align align)
{
	vlt_shader_program_bind(&gui->txt_shader);

	_set_color_attrib(&gui->txt_shader, c);
	_set_win_halfdim_attrib(gui, &gui->txt_shader);

	vlt_font *font = _get_font(gui, sz);
	assert(font);
	vlt_font_render_ex(font, txt, x, y, &gui->txt_shader, align);

	vlt_shader_program_unbind();
}

void vlt_gui_txt(vlt_gui *gui, s32 x, s32 y, s32 sz, const char *txt,
                 vlt_color c, font_align align)
{
	_vlt_gui_txt(gui, &x, &y, sz, txt, c, align);
}

void vlt_gui_mask(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h)
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glStencilMask(0xFF);
	vlt_gui_rect(gui, x, y, w, h, g_black, g_black);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilMask(0x00);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
}

void vlt_gui_unmask(vlt_gui *gui)
{
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00);
}

static
b8 _allow_new_interaction(const vlt_gui *gui)
{
	return !vlt_gui_mouse_down(gui,
		VLT_MB_LEFT | VLT_MB_MIDDLE | VLT_MB_RIGHT);
}

typedef enum _widget_style
{
	INACTIVE,
	HOT,
	ACTIVE
} _widget_style;

static
void _widget_color(const vlt_gui *gui, u64 id,
                   vlt_color *fill, vlt_color *outline, vlt_color *text)
{
	if (gui->active_id == id)
	{
		if (fill)
			*fill = gui->style.active_color;
		if (outline)
			*outline = gui->style.active_line_color;
		if (text)
			*text = gui->style.active_text_color;
	}
	else if (gui->hot_id == id)
	{
		if (fill)
			*fill = gui->style.hot_color;
		if (outline)
			*outline = gui->style.hot_line_color;
		if (text)
			*text = gui->style.hot_text_color;
	}
	else
	{
		if (fill)
			*fill = gui->style.fill_color;
		if (outline)
			*outline = gui->style.outline_color;
		if (text)
			*text = gui->style.text_color;
	}
}

static
b8 _repeat(vlt_gui *gui)
{
	if (gui->repeat_timer <= gui->frame_time_milli)
	{
		gui->repeat_timer = gui->repeat_interval -
			(gui->frame_time_milli - gui->repeat_timer);
		return true;
	}
	else
		gui->repeat_timer -= gui->frame_time_milli;
	return false;
}

b8 vlt_gui_npt(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
               char *txt, u32 n, font_align align, vlt_npt_flags flags)
{
	const u64 id = (u64)txt;
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	const b8 contains_mouse = box2i_contains_point(&box, &gui->mouse_pos);
	b8 complete = false;
	if (gui->active_id == id)
	{
		if (gui->key != 0)
		{
			b8 modify = false;
			if (gui->key != gui->prev_key)
			{
				modify = true;
				gui->repeat_timer = gui->repeat_delay;
			}
			else if (_repeat(gui))
				modify = true;
			if (modify)
			{
				const u32 len = strlen(txt);
				if (gui->key == KEY_BACKSPACE)
				{
					if (len > 0)
						txt[len-1] = '\0';
				}
				else if (gui->key == KEY_RETURN)
				{
					gui->active_id = 0;
					complete = true;
				}
				else
				{
					char buf[2] = { gui->key, '\0' };
					strncat(txt, buf, n-len-1);
				}
			}
		}
		else
			gui->repeat_timer = gui->repeat_delay;
		if (   vlt_gui_mouse_press(gui, VLT_MB_LEFT)
			&& !contains_mouse)
		{
			gui->active_id = 0;
		}
	}
	else if (gui->hot_id == id)
	{
		if (!contains_mouse)
		{
			gui->hot_id = 0;
		}
		else if (vlt_gui_mouse_press(gui, VLT_MB_LEFT))
		{
			gui->active_id = id;
			gui->hot_id = 0;
		}
	}
	else if (   _allow_new_interaction(gui)
	         && contains_mouse)
	{
		gui->hot_id = id;
	}


	vlt_color fill, line, text_color;
	_widget_color(gui, id, &fill, &line, &text_color);
	vlt_gui_rect(gui, x, y, w, h, fill, line);
	switch (align)
	{
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
	if (flags & VLT_NPT_PASSWORD)
	{
		const u32 sz = strlen(txt);
		array_reserve(&gui->pw_buf, sz+1);
		char *pw = gui->pw_buf.data;
		for (u32 i = 0; i < sz; ++i)
			pw[i] = '*';
		pw[sz] = '\0';
		_vlt_gui_txt(gui, &x, &txt_y, h*gui->style.font_ratio, pw,
			text_color, align);
	}
	else
	{
		_vlt_gui_txt(gui, &x, &txt_y, h*gui->style.font_ratio, txt,
			text_color, align);
	}
	if (gui->active_id == id)
	{
		const u32 milli_since_creation =
			vlt_diff_milli(&gui->creation_time, &gui->frame_start_time);
		if (milli_since_creation % 1000 < 500)
			vlt_gui_line(gui, x+1, txt_y, x+1, y+h-2, 1, text_color);
	}
	return complete;
}

static
vlt_btn_val _vlt_gui_btn_logic(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                               const char *txt, u64 id, b8 *contains_mouse)
{
	vlt_btn_val retval = VLT_BTN_NONE;
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	*contains_mouse = box2i_contains_point(&box, &gui->mouse_pos);
	if (gui->hot_id == id)
	{
		if (!*contains_mouse)
		{
			gui->hot_id = 0;
		}
		else if (vlt_gui_mouse_press(gui, VLT_MB_LEFT))
		{
			gui->hot_id = 0;
			gui->active_id = id;
			gui->repeat_timer = gui->repeat_delay;
		}
	}
	else if (gui->active_id == id)
	{
		if (vlt_gui_mouse_release(gui, VLT_MB_LEFT))
		{
			if (*contains_mouse)
				retval = VLT_BTN_PRESS;
			gui->active_id = 0;
			gui->repeat_timer = gui->repeat_delay;
		}
		else if (!*contains_mouse)
			gui->repeat_timer = gui->repeat_delay;
		else if (_repeat(gui))
			retval = VLT_BTN_HOLD;
	}
	else if (   _allow_new_interaction(gui)
	         && *contains_mouse)
	{
		gui->hot_id = id;
	}
	return retval;
}

static
void _vlt_gui_btn_render(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                         const char *txt, vlt_color fill,
                         vlt_color text_color)
{
	vlt_gui_rect(gui, x, y, w, h, fill, gui->style.outline_color);
	const s32 txt_y = y + h*(1.f-gui->style.font_ratio)/2.f;
	vlt_gui_txt(gui, x+w/2, txt_y, h*gui->style.font_ratio, txt,
		text_color, FONT_ALIGN_CENTER);
}

vlt_btn_val vlt_gui_btn(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                        const char *txt)
{
	const u64 _x = x, _y = y;
	const u64 id = (_x << 48) | (_y << 32) | vlt_hash(txt);
	b8 contains_mouse;
	const vlt_btn_val ret = _vlt_gui_btn_logic(gui, x, y, w, h, txt, id,
		&contains_mouse);

	vlt_color c = gui->style.fill_color;
	vlt_color text_color = gui->style.text_color;
	if (gui->active_id == id)
	{
		if (contains_mouse)
		{
			c = gui->style.active_color;
			text_color = gui->style.active_text_color;
		}
		else
		{
			c = gui->style.hot_color;
			text_color = gui->style.hot_text_color;
		}
	}
	else if (gui->hot_id == id)
	{
		c = gui->style.hot_color;
		text_color = gui->style.hot_text_color;
	}
	_vlt_gui_btn_render(gui, x, y, w, h, txt, c, text_color);
	return ret;
}

void vlt_gui_chk(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h, const char *txt,
                 b8 *val)
{
	const u64 _x = x, _y = y;
	const u64 id = (_x << 48) | (_y << 32) | vlt_hash(txt);
	b8 contains_mouse;
	const b8 was_checked = *val;
	if (_vlt_gui_btn_logic(gui, x, y, w, h, txt, id, &contains_mouse)
	        == VLT_BTN_PRESS)
		*val = !*val;

	vlt_color c = gui->style.fill_color;
	vlt_color text_color = gui->style.text_color;
	if (gui->active_id == id || (*val && !was_checked))
	{
		c = gui->style.active_color;
		text_color = gui->style.active_text_color;
	}
	if (gui->hot_id == id && contains_mouse)
	{
		c = gui->style.hot_color;
		text_color = gui->style.hot_text_color;
	}
	else if (*val)
	{
		c = gui->style.active_color;
		text_color = gui->style.active_text_color;
	}
	_vlt_gui_btn_render(gui, x, y, w, h, txt, c, text_color);
}

static
void _slider(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
             s32 hnd_len, r32 *val)
{
	assert(*val >= 0.f && *val <= 1.f);
	assert(w != h);

	const u64 id = (u64)val;
	const b8 vert = w < h;
	box2i box;
	if (vert)
	{
		box.min = (v2i){ .x=x, .y=y+(h-hnd_len)**val };
		box.max = (v2i){ .x=x+w, .y=y+hnd_len+(h-hnd_len)**val };
	}
	else
	{
		box.min = (v2i){ .x=x+(w-hnd_len)**val, .y=y };
		box.max = (v2i){ .x=x+hnd_len+(w-hnd_len)**val, .y=y+h };
	}
	const b8 contains_mouse = box2i_contains_point(&box, &gui->mouse_pos);

	if (gui->hot_id == id)
	{
		if (!contains_mouse)
		{
			gui->hot_id = 0;
		}
		else if (vlt_gui_mouse_press(gui, VLT_MB_LEFT))
		{
			gui->hot_id = 0;
			gui->active_id = id;
			gui->drag_offset.x =
				box.min.x+(box.max.x-box.min.x)/2-gui->mouse_pos.x;
			gui->drag_offset.y =
				box.min.y+(box.max.y-box.min.y)/2-gui->mouse_pos.y;
		}
	}
	else if (gui->active_id == id)
	{
		if (!vlt_gui_mouse_release(gui, VLT_MB_LEFT))
		{
			if (vert)
			{
				const r32 min_y = y+hnd_len/2;
				const r32 max_y = y+h-hnd_len/2;
				const s32 mouse_y = gui->mouse_pos.y+gui->drag_offset.y;
				*val = mathf_clamp(0, (mouse_y-min_y)/(max_y-min_y), 1.f);
			}
			else
			{
				const r32 min_x = x+hnd_len/2;
				const r32 max_x = x+w-hnd_len/2;
				const s32 mouse_x = gui->mouse_pos.x+gui->drag_offset.x;
				*val = mathf_clamp(0, (mouse_x-min_x)/(max_x-min_x), 1.f);
			}
		}
		else
			gui->active_id = 0;
	}
	else if (   _allow_new_interaction(gui)
	         && contains_mouse)
	{
		gui->hot_id = id;
	}

	const vlt_color outline = gui->style.outline_color;
	if (vert)
	{
		vlt_gui_line(gui, x+w/2, y+hnd_len/2, x+w/2, y+h-hnd_len/2, 1,
			outline);
	}
	else
	{
		vlt_gui_line(gui, x+hnd_len/2, y+h/2, x+w-hnd_len/2, y+h/2, 1,
			outline);
	}
	vlt_color c = gui->style.fill_color;
	if (gui->active_id == id)
		c = gui->style.active_color;
	else if (gui->hot_id == id)
		c = gui->style.hot_color;
	if (vert)
		vlt_gui_rect(gui, x, y+(h-hnd_len)**val, w, hnd_len, c, outline);
	else
		vlt_gui_rect(gui, x+(w-hnd_len)**val, y, hnd_len, h, c, outline);
}

void vlt_gui_slider(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h, r32 *val)
{
	const s32 hnd_len = min(w, h);
	_slider(gui, x, y, w, h, hnd_len, val);
}

void vlt_gui_select(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                    const char *txt, u32 *val, u32 opt)
{
	const b8 selected = *val == opt;
	const u64 id = (u64)val + opt;
	b8 contains_mouse;
	if (   _vlt_gui_btn_logic(gui, x, y, w, h, txt, id, &contains_mouse)
	           == VLT_BTN_PRESS
	    && !selected)
	{
		*val = opt;
	}

	vlt_color c = gui->style.fill_color;
	vlt_color text_color = gui->style.text_color;
	if (gui->active_id == id || *val == opt)
	{
		c = gui->style.active_color;
		text_color = gui->style.active_text_color;
	}
	if (gui->hot_id == id && contains_mouse)
	{
		c = gui->style.hot_color;
		text_color = gui->style.hot_text_color;
	}
	_vlt_gui_btn_render(gui, x, y, w, h, txt, c, text_color);
}

void vlt_gui_mselect(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                     const char *txt, u32 *val, u32 opt)
{
	const u64 id = (u64)val + opt;
	b8 contains_mouse;
	if (_vlt_gui_btn_logic(gui, x, y, w, h, txt, id, &contains_mouse)
	      == VLT_BTN_PRESS)
	{
	  if (!(*val & opt))
			*val |= opt;
		else if (*val & ~opt)
			*val &= ~opt;
	}

	vlt_color c = gui->style.fill_color;
	vlt_color text_color = gui->style.text_color;
	if (gui->active_id == id || (*val & opt))
	{
		c = gui->style.active_color;
		text_color = gui->style.active_text_color;
	}
	if (gui->hot_id == id && contains_mouse)
	{
		c = gui->style.hot_color;
		text_color = gui->style.hot_text_color;
	}
	_vlt_gui_btn_render(gui, x, y, w, h, txt, c, text_color);
}

static
b8 _vlt_gui_drag(vlt_gui *gui, s32 *x, s32 *y, b8 contains_mouse,
                 vlt_mb mb, u64 *_id)
{
	b8 retval = false;
	const u64 id = (u64)x;
	*_id = id;
	if (gui->hot_id == id)
	{
		if (!contains_mouse)
		{
			gui->hot_id = 0;
		}
		else if (vlt_gui_mouse_press(gui, mb))
		{
			gui->hot_id = 0;
			gui->active_id = id;
			gui->drag_offset.x = *x - gui->mouse_pos.x;
			gui->drag_offset.y = *y - gui->mouse_pos.y;
		}
	}
	else if (gui->active_id == id)
	{
		*x = gui->mouse_pos.x + gui->drag_offset.x;
		*y = gui->mouse_pos.y + gui->drag_offset.y;
		retval = true;
		if (vlt_gui_mouse_release(gui, mb))
			gui->active_id = 0;
	}
	else if (   _allow_new_interaction(gui)
	         && contains_mouse)
	{
		gui->hot_id = id;
	}
	return retval;
}

b8 vlt_gui_drag(vlt_gui *gui, s32 *x, s32 *y, u32 w, u32 h, vlt_mb mb)
{
	box2i box;
	box2i_from_dims(&box, *x, *y+h, *x+w, *y);
	const b8 contains_mouse = box2i_contains_point(&box, &gui->mouse_pos);
	u64 id;
	const b8 ret=_vlt_gui_drag(gui, x, y, contains_mouse, mb, &id);
	vlt_color fill, outline;
	_widget_color(gui, id, &fill, &outline, NULL);
	vlt_gui_rect(gui, *x, *y, w, h, fill, outline);
	return ret;
}

b8 vlt_gui_cdrag(vlt_gui *gui, s32 *x, s32 *y, u32 r, vlt_mb mb)
{
	const v2i pos = { .x=*x, .y=*y };
	const b8 contains_mouse = v2i_dist_sq(&pos, &gui->mouse_pos) <= r*r;
	u64 id;
	const b8 ret=_vlt_gui_drag(gui, x, y, contains_mouse, mb, &id);
	vlt_color fill, outline;
	_widget_color(gui, id, &fill, &outline, NULL);
	vlt_gui_circ(gui, *x, *y, r, fill, outline);
	return ret;
}

void vlt_gui_scrollbar(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                       s32 ttl_dim, r32 *off)
{
	assert(w != h);
	assert(ttl_dim > max(w, h));

	if (w < h)
		*off = 1.f - *off;

	const s32 miss = ttl_dim-max(w, h);
	const s32 hnd_len = max(min(w, h), max(w, h)-miss);
	_slider(gui, x, y, w, h, hnd_len, off);

	if (w < h)
		*off = 1.f - *off;
}

const vlt_style *vlt_gui_get_style(vlt_gui *gui)
{
	return &gui->style;
}

void vlt_gui_style(vlt_gui *gui, const vlt_style *style)
{
	gui->style = *style;
}

void vlt_gui_style_default(vlt_gui *gui)
{
	vlt_gui_style(gui, &gui->default_style);
}

