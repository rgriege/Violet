#ifndef SDL_WINDOW_H
#define SDL_WINDOW_H

#include "violet/core/window/Window.h"

#include <memory>

#ifdef WIN32
#ifdef VIOLETSDL_EXPORT
#define VIOLET_SDL_API __declspec(dllexport)
#else
#define VIOLET_SDL_API __declspec(dllimport)
#endif
#else
#define VIOLET_API
#endif

struct SDL_Window;
typedef void * SDL_GLContext;

namespace Violet
{
	class Deserializer;

	class VIOLET_SDL_API SDLWindow : public Window
	{
	public:

		static std::unique_ptr<Window> create(Deserializer & deserializer);

	public:

		virtual ~SDLWindow() override;

		virtual bool update() override;
		virtual void render() override;
		virtual bool getEvent(EventType type, Event* event) override;
		virtual void addEvent(Event event) override;
		virtual int getWidth() const override;
		virtual int getHeight() const override;

	private:

		SDLWindow(SDL_Window * window, SDL_GLContext context);

	private:

		SDL_Window * m_window;
		SDL_GLContext m_glContext;
		int m_width;
		int m_height;
		bool m_quit;
	};
}

#endif