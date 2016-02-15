// ============================================================================

#include "violet/window/sdl/sdl_window_system.h"

#include "violet/core/engine.h"
#include "violet/graphics/system/render_system.h"
#include "violet/input/key.h"
#include "violet/log/Log.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"

#include <SDL.h>

#include <assert.h>

using namespace vlt;

// ============================================================================

namespace SDLWindowSystemNamespace
{
	char convertKey(SDL_Keycode key);
	key::modifier convertKeyModifier(SDL_Keymod mod);
	int filterEvent(void * userdata, SDL_Event * event);
}

using namespace SDLWindowSystemNamespace;

// ============================================================================

void sdl_window_system::install(system_factory & factory)
{
	factory.assign(get_label_static(), &sdl_window_system::init);
}

// ----------------------------------------------------------------------------

namespace
{
	struct init_task_data
	{
		int x;
		int y;
		int width;
		int height;
		std::string title;
		u32 thread;
	};
}

static void init_task(void * mem)
{
	auto data = make_unique<init_task_data>(mem);
	auto & engine = engine::instance();
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		log(formatted_string<1024>().sprintf("SDL_Init(VIDEO) failed: %s", SDL_GetError()));
		engine.stop();
		return;
	}

	//Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window * window = SDL_CreateWindow(data->title.c_str(), data->x, data->y, data->width, data->height, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		log(formatted_string<1024>().sprintf("SDL_CreateWindow failed: %s", SDL_GetError()));
		engine.stop();
		return;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
	{
		log(formatted_string<1024>().sprintf("SDL_CreateContext failed: %s", SDL_GetError()));
		engine.stop();
		return;
	}

	SDL_SetEventFilter(filterEvent, nullptr);

	engine.add_system(std::unique_ptr<vlt::system>(new sdl_window_system(window, glContext, data->thread)));
}

void sdl_window_system::init(deserializer & deserializer)
{
	auto settingsSegment = deserializer.enter_segment(get_label_static());

	auto data = new init_task_data;
	data->x = settingsSegment->get_s32("x");
	data->y = settingsSegment->get_s32("y");
	data->width = settingsSegment->get_s32("width");
	data->height = settingsSegment->get_s32("height");
	data->title = settingsSegment->get_string("title");
	data->thread = settingsSegment->get_u32("thread");

	add_task(init_task, data, data->thread, task_type::write);
}

// ============================================================================

sdl_window_system::sdl_window_system(SDL_Window * window, SDL_GLContext context, u32 thread) :
	window_system(),
	m_window(window),
	m_glContext(context),
	m_width(0),
	m_height(0),
	m_thread(thread)
{
	SDL_GetWindowSize(m_window, &m_width, &m_height);
}

// ----------------------------------------------------------------------------

struct destroy_data
{
	SDL_GLContext gl_context;
	SDL_Window * window;
};

static void destroy(void * mem)
{
	auto data = make_unique<destroy_data>(mem);
	SDL_GL_DeleteContext(data->gl_context);
	SDL_DestroyWindow(data->window);
	SDL_Quit();
}

sdl_window_system::~sdl_window_system()
{
	auto data = new destroy_data{ m_glContext, m_window };
	add_task(destroy, data, m_thread, task_type::del);
}

// ----------------------------------------------------------------------------

void sdl_window_system::render()
{
	SDL_GL_SwapWindow(m_window);
}

// ----------------------------------------------------------------------------

bool sdl_window_system::get_event(const event_type type, event * const event)
{
	bool queueEmpty = false;
	bool foundEvent = false;

	while (!queueEmpty && !foundEvent)
	{
		SDL_Event sdlEvent;
		if (SDL_PollEvent(&sdlEvent) == 1)
		{
			switch (sdlEvent.type)
			{
			case SDL_KEYDOWN:
				if (type & ET_KeyDown)
				{
					event->type = window_system::ET_KeyDown;
					event->key.code = convertKey(sdlEvent.key.keysym.sym);
					event->key.modifiers = convertKeyModifier(SDL_GetModState());
					foundEvent = true;
				}
				break;

			case SDL_KEYUP:
				if (type & ET_KeyUp)
				{
					event->type = window_system::ET_KeyUp;
					event->key.code = convertKey(sdlEvent.key.keysym.sym);
					event->key.modifiers = convertKeyModifier(SDL_GetModState());
					foundEvent = true;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (type & ET_MouseDown)
				{
					event->type = window_system::ET_MouseDown;
					event->mouse.x = sdlEvent.button.x;
					event->mouse.y = sdlEvent.button.y;
					event->mouse.button = static_cast<mouse_button>(sdlEvent.button.button - 1);
					event->mouse.modifiers = convertKeyModifier(SDL_GetModState());
					foundEvent = true;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if (type & ET_MouseUp)
				{
					event->type = window_system::ET_MouseUp;
					event->mouse.x = sdlEvent.button.x;
					event->mouse.y = sdlEvent.button.y;
					event->mouse.button = static_cast<mouse_button>(sdlEvent.button.button - 1);
					event->mouse.modifiers = convertKeyModifier(SDL_GetModState());
					foundEvent = true;
				}
				break;

			case SDL_MOUSEMOTION:
				if (type & ET_MouseMove)
				{
					event->type = window_system::ET_MouseMove;
					event->motion.x = sdlEvent.motion.x;
					event->motion.y = sdlEvent.motion.y;
					event->motion.xrel = sdlEvent.motion.xrel;
					event->motion.yrel = sdlEvent.motion.yrel;
					foundEvent = true;
				}
				break;

			case SDL_QUIT:
				if (type & ET_Quit)
				{
					event->type = window_system::ET_Quit;
					foundEvent = true;
				}
				break;
			}
		}
		else
			queueEmpty = true;
	}

	return foundEvent;
}

// ----------------------------------------------------------------------------

void sdl_window_system::add_event(event event)
{
	SDL_Event sdlEvent;
	switch (event.type)
	{
	case window_system::ET_KeyDown:
		sdlEvent.type = SDL_KEYDOWN;
		sdlEvent.key.keysym.sym = static_cast<SDL_Keycode>(event.key.code);
		break;

	case window_system::ET_KeyUp:
		sdlEvent.type = SDL_KEYUP;
		sdlEvent.key.keysym.sym = static_cast<SDL_Keycode>(event.key.code);
		break;

	case window_system::ET_MouseUp:
		sdlEvent.type = SDL_MOUSEBUTTONUP;
		sdlEvent.button.x = event.mouse.x;
		sdlEvent.button.y = event.mouse.y;
		break;

	case window_system::ET_MouseDown:
		sdlEvent.type = SDL_MOUSEBUTTONDOWN;
		sdlEvent.button.x = event.mouse.x;
		sdlEvent.button.y = event.mouse.y;
		break;

	case window_system::ET_MouseMove:
		sdlEvent.type = SDL_MOUSEMOTION;
		sdlEvent.motion.x = event.motion.x;
		sdlEvent.motion.y = event.motion.y;
		sdlEvent.motion.xrel = event.motion.xrel;
		sdlEvent.motion.yrel = event.motion.yrel;
		break;
	}
	SDL_PushEvent(&sdlEvent);
}

// ----------------------------------------------------------------------------

int sdl_window_system::get_width() const
{
	return m_width;
}

// ----------------------------------------------------------------------------

int sdl_window_system::get_height() const
{
	return m_height;
}

// ============================================================================

char SDLWindowSystemNamespace::convertKey(SDL_Keycode const key)
{
	if (key >= 0 && key <= 255)
		return key;
	else if (key >= SDLK_KP_DIVIDE && key <= SDLK_KP_PERIOD)
	{
		static char keys[1 + SDLK_KP_PERIOD - SDLK_KP_DIVIDE] = { '/', '*', '-', '+', key::Return, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.' };
		return keys[key - SDLK_KP_DIVIDE];
	}
	else if (key == 10)
		return key::Return;

	return 0;
}

// ----------------------------------------------------------------------------

key::modifier SDLWindowSystemNamespace::convertKeyModifier(const SDL_Keymod mod)
{
	return static_cast<key::modifier>(mod & 0x3 | (mod & 0x3c0) >> 4 | (mod & 0x3000) >> 6);
}

// ----------------------------------------------------------------------------

int SDLWindowSystemNamespace::filterEvent(void * /*userdata*/, SDL_Event * event)
{
	switch (event->type)
	{
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEMOTION:
	case SDL_QUIT:
		return 1;

	default:
		return 0;
		break;
	}
}

// ============================================================================
