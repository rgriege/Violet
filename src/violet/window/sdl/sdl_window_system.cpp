// ============================================================================

#include "violet/window/sdl/sdl_window_system.h"

#include "violet/core/engine.h"
#include "violet/graphics/system/render_system.h"
#include "violet/input/key.h"
#include "violet/log/Log.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/utility/formatted_string.h"

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

void sdl_window_system::init(deserializer & deserializer)
{
	auto settingsSegment = deserializer.enter_segment(get_label_static());

	const int x = settingsSegment->get_s32("x");
	const int y = settingsSegment->get_s32("y");
	const int width = settingsSegment->get_s32("width");
	const int height = settingsSegment->get_s32("height");
	const std::string title = settingsSegment->get_string("title");

	engine::instance().add_write_task(engine::instance(),
		[=](engine & engine)
		{
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

			SDL_Window * window = SDL_CreateWindow(title.c_str(), x, y, width, height, SDL_WINDOW_OPENGL);
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

			engine.add_system(std::unique_ptr<system>(new sdl_window_system(window, glContext)));
		}, thread::Window);
}

// ============================================================================

sdl_window_system::~sdl_window_system()
{
	const auto glContext = m_glContext;
	const auto window = m_window;
	engine::instance().add_delete_task(std::make_unique<delegate_task>(
		[=]()
		{
			SDL_GL_DeleteContext(glContext);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}), thread::Window);
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

sdl_window_system::sdl_window_system(SDL_Window * window, SDL_GLContext context) :
	window_system(),
	m_window(window),
	m_glContext(context),
	m_width(0),
	m_height(0)
{
	SDL_GetWindowSize(m_window, &m_width, &m_height);
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
