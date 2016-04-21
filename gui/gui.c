#include <string.h>

#include <GL/glew.h>
#include <SDL.h>

#include "violet/math/aabb.h"
#include "violet/math/ibox.h"
#include "violet/math/math.h"
#include "violet/math/poly.h"
#include "violet/utility/log.h"

#include "violet/gui/font.h"
#include "violet/gui/gui.h"
#include "violet/gui/img.h"
#include "violet/gui/mesh.h"
#include "violet/gui/shader.h"
#include "violet/gui/texture.h"

int SDL_FilterEvent(void * userdata, SDL_Event * event)
{
	switch (event->type)
	{
	//case SDL_KEYDOWN:
	//case SDL_KEYUP:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	//case SDL_MOUSEMOTION:
	case SDL_QUIT:
		return 1;

	default:
		return 0;
		break;
	}
}

typedef enum key
{
	KEY_RETURN = 13,
	KEY_BACKSPACE = 8
} key;

b8 _convert_scancode(SDL_Scancode code, char * c)
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

typedef struct vlt_gui
{
	SDL_Window * window;
	SDL_GLContext gl_context;
	vlt_shader_program poly_shader;
	vlt_shader_program tex_shader;
	vlt_shader_program txt_shader;
	v2i win_halfdim;
	v2i mouse_pos;
	u32 mouse_btn;
	char key;
	vlt_font * font;
} vlt_gui;

vlt_gui * vlt_gui_create()
{
	return calloc(1, sizeof(vlt_gui));
}

b8 vlt_gui_init_window(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h,
                       vlt_color c, const char * title)
{
	b8 retval = 0;
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		log_write("SDL_Init(VIDEO) failed: %s", SDL_GetError());
		goto out;
	}

	// Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
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

	float color[4];
	vlt_color_as_float_array(color, c);
	glClearColor(color[0], color[1], color[2], color[3]);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!vlt_shader_program_load(&gui->poly_shader, "poly"))
		goto err_glew;

	if (!vlt_shader_program_load(&gui->tex_shader, "texu"))
		goto err_poly;

	if (!vlt_shader_program_load(&gui->txt_shader, "text"))
		goto err_texu;

	if (!vlt_font_install())
		goto err_text;

	gui->font = vlt_font_create();
	if (!vlt_font_load(gui->font, "MyriadProRegular.ttf", 45))
		goto err_font;

	retval = 1;
	goto out;

err_font:
	vlt_font_destroy(gui->font);
	vlt_font_uninstall();
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
out:
	return retval;
}

void vlt_gui_destroy_window(vlt_gui * gui)
{
	vlt_font_destroy(gui->font);
	vlt_font_free(gui->font);
	vlt_font_uninstall();
	vlt_shader_program_destroy(&gui->poly_shader);
	vlt_shader_program_destroy(&gui->tex_shader);
	vlt_shader_program_destroy(&gui->txt_shader);
	SDL_GL_DeleteContext(gui->gl_context);
	SDL_DestroyWindow(gui->window);
	SDL_Quit();
}

void vlt_gui_destroy(vlt_gui * gui)
{
	free(gui);
}

static void _set_color_attrib(vlt_shader_program * p, vlt_color c)
{
	const GLint color_attrib = vlt_shader_program_uniform(p, "color");
	float color_f[4];
	vlt_color_as_float_array(color_f, c);
	glUniform4fv(color_attrib, 1, color_f);
}

static void _set_win_halfdim_attrib(vlt_gui * gui, vlt_shader_program * p)
{
	const GLint win_attrib = vlt_shader_program_uniform(p,
		"window_halfdim");
	glUniform2f(win_attrib, gui->win_halfdim.x, gui->win_halfdim.y);
}

void vlt_gui_rect(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h, vlt_color c, vlt_color lc)
{
	aabb box;
	aabb_from_dims(&box, x, y + h, x + w, y);
	poly p;
	poly_from_box(&p, &box);
	vlt_mesh mesh;
	vlt_mesh_init(&mesh, &p.vertices);

	u32 vao;
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	vlt_mesh_bind(&mesh);

	const GLint pos_attrib = vlt_shader_program_attrib(&gui->poly_shader, "position");
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pos_attrib);

	vlt_shader_program_bind(&gui->poly_shader);

	_set_color_attrib(&gui->poly_shader, c);
	_set_win_halfdim_attrib(gui, &gui->poly_shader);

	glDrawArrays(GL_TRIANGLE_FAN, 0, mesh.sz);
	_set_color_attrib(&gui->poly_shader, lc);
	glDrawArrays(GL_LINE_LOOP, 0, mesh.sz);

	vlt_shader_program_unbind();

	glBindVertexArray(0);
	vlt_mesh_unbind(&mesh);

	glDeleteVertexArrays(1, &vao);

	vlt_mesh_destroy(&mesh);
}

void vlt_gui_img(vlt_gui * gui, s32 x, s32 y, const char * filename)
{
	vlt_img * img = vlt_img_create();
	if (!vlt_img_load(img, filename))
		return;

	vlt_shader_program_bind(&gui->tex_shader);

	_set_win_halfdim_attrib(gui, &gui->tex_shader);

	vlt_img_render(img, x, y, &gui->tex_shader);

	vlt_shader_program_unbind();

	vlt_img_destroy(img);
	vlt_img_free(img);
}

void vlt_gui_txt(vlt_gui * gui, s32 x, s32 y, s32 sz, const char * txt,
                 vlt_color c, font_align align)
{
	vlt_shader_program_bind(&gui->txt_shader);

	_set_color_attrib(&gui->txt_shader, c);
	_set_win_halfdim_attrib(gui, &gui->txt_shader);

	vlt_font_render(gui->font, txt, x, y, &gui->txt_shader, align);

	vlt_shader_program_unbind();
}

void vlt_gui_npt(vlt_gui * gui, s32 x, s32 y, s32 sz, char * txt, u32 n,
                 vlt_color c, font_align align)
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
			strncat(txt, buf, n - len - 1);
		}
	}

	vlt_gui_txt(gui, x, y, sz, txt, c, align);
}

b8 vlt_gui_btn(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h, vlt_color c, vlt_color lc)
{
	vlt_gui_rect(gui, x, y, w, h, c, lc);
	ibox box;
	ibox_from_dims(&box, x, y + h, x + w, y);
	return (gui->mouse_btn & SDL_BUTTON_LMASK)
	       && ibox_contains(&box, &gui->mouse_pos);
}

b8 vlt_gui_begin_frame(vlt_gui * gui)
{
	b8 quit = false;
	SDL_Event evt;
	while (SDL_PollEvent(&evt) == 1)
	{
		switch(evt.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}
	}

	gui->mouse_btn = SDL_GetMouseState(&gui->mouse_pos.x,
		&gui->mouse_pos.y);

	SDL_GetWindowSize(gui->window, &gui->win_halfdim.x,
		&gui->win_halfdim.y);
	v2i_div(&gui->win_halfdim, 2, 2, &gui->win_halfdim);

	gui->key = 0;
	s32 key_cnt;
	const u8 * keys = SDL_GetKeyboardState(&key_cnt);
	for (s32 i = 0; i < key_cnt; ++i)
		if (keys[i] && _convert_scancode(i, &gui->key))
			break;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return !quit;
}

void vlt_gui_end_frame(vlt_gui * gui)
{
	glFlush();
	SDL_GL_SwapWindow(gui->window);
}

