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
#include "violet/math/r32.h"
#include "violet/math/s32.h"
#include "violet/structures/array_map.h"
#include "violet/utility/log.h"
#include "violet/utility/time.h"

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

	if (*c == KEY_RETURN)
	{
		*c = 0;
		return false;
	}
	else
		return true;
}

typedef enum _hot_stage
{
	NONE,
	HOVER,
	INITIATE
} _hot_stage;

typedef struct vlt_gui
{
	vlt_time creation_time;
	vlt_time frame_start_time;
	SDL_Window *window;
	SDL_GLContext gl_context;
	vlt_shader_program poly_shader;
	vlt_shader_program tex_shader;
	vlt_shader_program txt_shader;
	v2i win_halfdim;
	v2i mouse_pos;
	u32 mouse_btn;
	u32 mouse_btn_diff;
	char _pressed_key;
	char key;
	u32 key_repeat_delay;
	u32 key_repeat_timer;
	array_map fonts; // sz -> vlt_font *
	vlt_style style;
	u64 hot_id;
	_hot_stage hot_stage;
	u64 active_id;
} vlt_gui;

vlt_gui *vlt_gui_create(s32 x, s32 y, s32 w, s32 h, const char *title)
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	gui->window = SDL_CreateWindow(title, max(x, 5), max(y, 30), w, h, SDL_WINDOW_OPENGL);
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

	gui->key_repeat_delay = 500;
	gui->key_repeat_timer = gui->key_repeat_delay;

	const vlt_color bg_color = { .r=0x37, .g=0x3e, .b=0x48, .a=0xff };
	gui->style.bg_color = bg_color;
	const vlt_color fill_color = { .r=0x1c, .g=0x1f, .b=0x24, .a=0xff };
	gui->style.fill_color = fill_color;
	gui->style.outline_color = g_nocolor;
	const vlt_color text_color = { .r=0xdb, .g=0xde, .b=0xe3, .a=0xff };
	gui->style.text_color = text_color;
	const vlt_color baseline_color = { .r=0x6f, .g=0x7c, .b=0x91, .a=0xff };
	gui->style.baseline_color = baseline_color;
	const vlt_color hot_color = { .r=0x0b, .g=0x17, .b=0x28, .a=0xff };
	gui->style.hot_color = hot_color;

	gui->hot_id = gui->active_id = 0;
	gui->hot_stage = NONE;

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
	array_map_destroy_each(&gui->fonts, NULL, _font_clear);
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

void vlt_gui_mouse_scroll(const vlt_gui *gui, s32 *dir)
{
	if (gui->mouse_btn & VLT_MB_WHEELUP)
		*dir = 1;
	else if (gui->mouse_btn & VLT_MB_WHEELDOWN)
		*dir = -1;
	else
		*dir = 0;
}

char vlt_gui_key(const vlt_gui *gui)
{
	return gui->key;
}

b8 vlt_gui_begin_frame(vlt_gui *gui)
{
	vlt_time now;
	vlt_get_time(&now);
	const u32 frame_milli = vlt_diff_milli(&gui->frame_start_time, &now);
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
	SDL_GetWindowSize(gui->window, &gui->win_halfdim.x,
		&gui->win_halfdim.y);
	gui->mouse_pos.y = gui->win_halfdim.y - gui->mouse_pos.y;
	static const v2i g_v2i_2 = { .x=2, .y=2 };
	v2i_div(&gui->win_halfdim, &g_v2i_2, &gui->win_halfdim);

	const char prev_key = gui->_pressed_key;
	gui->_pressed_key = 0;
	gui->key = 0;
	s32 key_cnt;
	const u8 *keys = SDL_GetKeyboardState(&key_cnt);
	for (s32 i = 0; i < key_cnt; ++i)
		if (keys[i] && _convert_scancode(i, &gui->_pressed_key))
			break;
	if (gui->_pressed_key != 0)
	{
		if (gui->_pressed_key == prev_key)
		{
			if (gui->key_repeat_timer <= frame_milli)
			{
				gui->key_repeat_timer = 0;
				gui->key = gui->_pressed_key;
			}
			else
				gui->key_repeat_timer -= frame_milli;
		}
		else
		{
			gui->key_repeat_timer = gui->key_repeat_delay;
			gui->key = gui->_pressed_key;
		}
	}

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

	const GLint pos_attrib = vlt_shader_program_attrib(&gui->poly_shader, "position");
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pos_attrib);
}

void vlt_rmgui_line_init(vlt_gui *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                         vlt_color c, vlt_rmgui_poly *line)
{
	v2f vertices[2] = {
		{ x0, y0 },
		{ x1, y1 }
	};

	vlt_rmgui_poly_init(gui, vertices, 2, &line->mesh, &line->vao);
	line->line_color = c;
	line->fill_color = g_nocolor;
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

	const GLint offset_attrib = vlt_shader_program_uniform(&gui->poly_shader,
		"offset");
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

void vlt_gui_line(vlt_gui *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                  vlt_color c)
{
	vlt_rmgui_poly line;
	vlt_rmgui_line_init(gui, x0, y0, x1, y1, c, &line);
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
	// TODO(rgriege): find either a version of Myriad that works on ubuntu
	// or a font that matches Myriad more closely
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

static
void _vlt_gui_txt_ex(vlt_gui *gui, s32 *x, s32 *y, s32 sz,
                     const char *txt, font_align align)
{
	vlt_shader_program_bind(&gui->txt_shader);

	_set_color_attrib(&gui->txt_shader, gui->style.text_color);
	_set_win_halfdim_attrib(gui, &gui->txt_shader);

	vlt_font *font = _get_font(gui, sz);
	assert(font);
	vlt_font_render_ex(font, txt, x, y, &gui->txt_shader, align);

	vlt_shader_program_unbind();
}

void vlt_gui_txt(vlt_gui *gui, s32 x, s32 y, s32 sz,
                 const char *txt, font_align align)
{
	_vlt_gui_txt_ex(gui, &x, &y, sz, txt, align);
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
	glClear(GL_STENCIL_BUFFER_BIT);
}

static
b8 _allow_new_interaction(const vlt_gui *gui)
{
	switch (gui->hot_stage)
	{
	case NONE:
	case HOVER:
		return !vlt_gui_mouse_down(gui,
			VLT_MB_LEFT | VLT_MB_MIDDLE | VLT_MB_RIGHT);
	case INITIATE:
		break;
	}
	return false;
}

void vlt_gui_npt(vlt_gui *gui, s32 x, s32 y, s32 w, s32 sz,
                 char *txt, u32 n, font_align align)
{
	const u64 id = (u64)txt;
	box2i box;
	box2i_from_dims(&box, x, y+sz, x+w, y);
	const b8 contains_mouse = box2i_contains_point(&box, &gui->mouse_pos);
	if (gui->active_id == id)
	{
		if (gui->key != 0)
		{
			const u32 len = strlen(txt);
			if (len > 0 && gui->key == KEY_BACKSPACE)
			{
				txt[len-1] = '\0';
			}
			else
			{
				char buf[2] = { gui->key, '\0' };
				strncat(txt, buf, n-len-1);
			}
		}
		if (   vlt_gui_mouse_press(gui, VLT_MB_LEFT)
			&& !contains_mouse)
		{
			gui->active_id = 0;
		}
	}
	else if (gui->hot_id == id)
	{
		switch (gui->hot_stage)
		{
		case NONE:
		case INITIATE:
			assert(false);
			break;
		case HOVER:
			if (!contains_mouse)
			{
				gui->hot_id = 0;
				gui->hot_stage = NONE;
			}
			else if (vlt_gui_mouse_press(gui, VLT_MB_LEFT))
			{
				gui->active_id = id;
				gui->hot_id = 0;
				gui->hot_stage = NONE;
			}
			break;
		}
	}
	else if (   _allow_new_interaction(gui)
	         && contains_mouse)
	{
		gui->hot_id = id;
		gui->hot_stage = HOVER;
	}


	const vlt_color c = gui->hot_id == id || gui->active_id == id
		? gui->style.text_color : gui->style.baseline_color;
	vlt_gui_line(gui, x, y-2, x+w, y-2, c);
	_vlt_gui_txt_ex(gui, &x, &y, sz, txt, align);
	if (gui->active_id == id)
	{
		const u32 milli_since_creation =
			vlt_diff_milli(&gui->creation_time, &gui->frame_start_time);
		if (milli_since_creation % 1000 < 500)
			vlt_gui_line(gui, x+1, y, x+1, y+sz, gui->style.text_color);
	}
}

b8 vlt_gui_btn(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h, const char *txt)
{
	b8 retval = false;
	const u64 id = (u64)txt;
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	const b8 contains_mouse = box2i_contains_point(&box, &gui->mouse_pos);
	if (gui->hot_id == id)
	{
		switch (gui->hot_stage)
		{
		case NONE:
			assert(false);
			break;
		case HOVER:
			if (!contains_mouse)
			{
				gui->hot_id = 0;
				gui->hot_stage = NONE;
			}
			else if (vlt_gui_mouse_press(gui, VLT_MB_LEFT))
			{
				gui->hot_stage = INITIATE;
			}
			break;
		case INITIATE:
			if (vlt_gui_mouse_release(gui, VLT_MB_LEFT))
			{
				if (contains_mouse)
				{
					gui->hot_stage = HOVER;
					retval = true;
				}
				else
				{
					gui->hot_id = 0;
					gui->hot_stage = NONE;
				}
			}
			break;
		}
	}
	else if (   _allow_new_interaction(gui)
	         && contains_mouse)
	{
		gui->hot_id = id;
		gui->hot_stage = HOVER;
	}


	const vlt_color c = gui->hot_id == id && contains_mouse
		? gui->style.hot_color : gui->style.fill_color;
	vlt_gui_rect(gui, x, y, w, h, c, gui->style.outline_color);
	vlt_gui_txt(gui, x+w/2, y, h, txt, FONT_ALIGN_CENTER);
	return retval;
}

void vlt_gui_slider(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h, r32 *val)
{
	assert(*val >= 0.f && *val <= 1.f);

	const u64 id = (u64)val;
	box2i box;
	{
		const v2i center = { .x=x+h/2+(w-h)**val, .y=y+h/2 };
		const v2i halfdim = { .x=h/2, .y=h/2 };
		box2i_from_center(&box, &center, &halfdim);
	}
	const b8 contains_mouse = box2i_contains_point(&box, &gui->mouse_pos);

	if (gui->hot_id == id)
	{
		switch (gui->hot_stage)
		{
		case NONE:
			assert(false);
		case HOVER:
			if (!contains_mouse)
			{
				gui->hot_id = 0;
				gui->hot_stage = NONE;
			}
			else if (vlt_gui_mouse_press(gui, VLT_MB_LEFT))
			{
				gui->hot_stage = INITIATE;
			}
			break;
		case INITIATE:
			if (!vlt_gui_mouse_release(gui, VLT_MB_LEFT))
			{
				const r32 min_x = x+h/2;
				const r32 max_x = x+w-h/2;
				*val = mathf_clamp(0,
					(gui->mouse_pos.x - min_x) / (max_x - min_x), 1.f);
			}
			else
			{
				gui->hot_id = 0;
				gui->hot_stage = NONE;
			}
			break;
		}
	}
	else if (   _allow_new_interaction(gui)
	         && contains_mouse)
	{
		gui->hot_id = id;
		gui->hot_stage = HOVER;
	}

	vlt_gui_line(gui, x+h/2, y+h/2, x+w-h/2, y+h/2,
		gui->style.baseline_color);
	const vlt_color c = gui->hot_id == id
		? gui->style.hot_color : gui->style.fill_color;
	vlt_gui_rect(gui, x+(w-h)**val, y, h, h, c, gui->style.outline_color);
}

vlt_style *vlt_gui_style(vlt_gui *gui)
{
	return &gui->style;
}

