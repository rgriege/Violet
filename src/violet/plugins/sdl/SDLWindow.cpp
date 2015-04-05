#include "violet/plugins/sdl/SDLWindow.h"

#include "violet/core/serialization/Deserializer.h"

#include <SDL.h>

#include <assert.h>
#include <iostream>

using namespace Violet;

namespace SDLWindowNamespace
{
	char convertKey(SDL_Keycode key);
	int filterEvent(void * userdata, SDL_Event * event);
}

using namespace SDLWindowNamespace;

std::unique_ptr<Window> SDLWindow::create(Deserializer & deserializer)
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL_Init(VIDEO) failed: " << SDL_GetError() << std::endl;
		return nullptr;
	}

	int const x = deserializer.getInt("x");
	int const y = deserializer.getInt("y");
	int const width = deserializer.getInt("width");
	int const height = deserializer.getInt("height");
	const char * title = deserializer.getString("title");

	//Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window * window = SDL_CreateWindow(title, x, y, width, height, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
		return nullptr;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
	{
		std::cout << "SDL_CreateContext failed: " << SDL_GetError() << std::endl;
		return nullptr;
	}

	SDL_SetEventFilter(filterEvent, nullptr);

	return std::unique_ptr<Window>(new SDLWindow(window, glContext));
}

SDLWindow::~SDLWindow()
{
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void SDLWindow::render()
{
	SDL_GL_SwapWindow(m_window);
}

bool SDLWindow::update()
{
	return !m_quit;
}

bool SDLWindow::getEvent(EventType type, Event* event)
{
	SDL_Event sdlEvent;
	bool const hasEvent = SDL_PollEvent(&sdlEvent) == 1;
	if (hasEvent)
	{
		switch (sdlEvent.type)
		{
		case SDL_KEYDOWN:
			event->type = Window::ET_KeyDown;
			event->key.code = convertKey(sdlEvent.key.keysym.sym);
			break;

		case SDL_KEYUP:
			event->type = Window::ET_KeyUp;
			event->key.code = convertKey(sdlEvent.key.keysym.sym);
			break;

		case SDL_MOUSEBUTTONDOWN:
			event->type = Window::ET_MouseDown;
			event->mouse.x = sdlEvent.button.x;
			event->mouse.y = sdlEvent.button.y;
			break;

		case SDL_MOUSEBUTTONUP:
			event->type = Window::ET_MouseUp;
			event->mouse.x = sdlEvent.button.x;
			event->mouse.y = sdlEvent.button.y;
			break;

		case SDL_MOUSEMOTION:
			event->type = Window::ET_MouseMove;
			event->motion.x = sdlEvent.motion.x;
			event->motion.y = sdlEvent.motion.y;
			event->motion.xrel = sdlEvent.motion.xrel;
			event->motion.yrel = sdlEvent.motion.yrel;
			break;

		case SDL_QUIT:
			m_quit = true;
			break;
		}
	}

	return hasEvent;
}

void SDLWindow::addEvent(Event event)
{
	SDL_Event sdlEvent;
	switch (event.type)
	{
	case Window::ET_KeyDown:
		sdlEvent.type = SDL_KEYDOWN;
		sdlEvent.key.keysym.sym = static_cast<SDL_Keycode>(event.key.code);
		break;

	case Window::ET_KeyUp:
		sdlEvent.type = SDL_KEYUP;
		sdlEvent.key.keysym.sym = static_cast<SDL_Keycode>(event.key.code);
		break;

	case Window::ET_MouseUp:
		sdlEvent.type = SDL_MOUSEBUTTONUP;
		sdlEvent.button.x = event.mouse.x;
		sdlEvent.button.y = event.mouse.y;
		break;

	case Window::ET_MouseDown:
		sdlEvent.type = SDL_MOUSEBUTTONDOWN;
		sdlEvent.button.x = event.mouse.x;
		sdlEvent.button.y = event.mouse.y;
		break;

	case Window::ET_MouseMove:
		sdlEvent.type = SDL_MOUSEMOTION;
		sdlEvent.motion.x = event.motion.x;
		sdlEvent.motion.y = event.motion.y;
		sdlEvent.motion.xrel = event.motion.xrel;
		sdlEvent.motion.yrel = event.motion.yrel;
		break;
	}
	SDL_PushEvent(&sdlEvent);
}

int SDLWindow::getWidth() const
{
	return m_width;
}

int SDLWindow::getHeight() const
{
	return m_height;
}

SDLWindow::SDLWindow(SDL_Window * window, SDL_GLContext context) :
	m_window(window),
	m_glContext(context),
	m_width(0),
	m_height(0),
	m_quit(false)
{
	SDL_GetWindowSize(m_window, &m_width, &m_height);
}

char SDLWindowNamespace::convertKey(SDL_Keycode key)
{
	static char keys[1 + SDLK_z - SDLK_a] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
	return (key >= SDLK_a && key <= SDLK_z) ? keys[key - SDLK_a] : 0;
}

int SDLWindowNamespace::filterEvent(void * /*userdata*/, SDL_Event * event)
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