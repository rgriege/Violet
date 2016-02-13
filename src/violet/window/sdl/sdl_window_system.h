#ifndef VIOLET_SDL_WINDOW_SYSTEM_H
#define VIOLET_SDL_WINDOW_SYSTEM_H

#include <memory>

#include "violet/core/defines.h"
#include "violet/window/window_system.h"

struct SDL_Window;
typedef void * SDL_GLContext;

namespace vlt
{
	struct deserializer;
	struct engine;
	struct system_factory;

	struct VIOLET_API sdl_window_system final : public window_system
	{
		static void install(system_factory & factory);
		static void init(deserializer & deserializer);

		virtual ~sdl_window_system() override;

		virtual void render() override;
		virtual bool get_event(event_type type, event* event) override;
		virtual void add_event(event event) override;
		virtual int get_width() const override;
		virtual int get_height() const override;

	private:

		sdl_window_system(SDL_Window * window, SDL_GLContext context);

	private:

		SDL_Window * m_window;
		SDL_GLContext m_glContext;
		int m_width;
		int m_height;
	};
}

#endif