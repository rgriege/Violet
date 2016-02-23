#ifndef VIOLET_SDL_WINDOW_SYSTEM_H
#define VIOLET_SDL_WINDOW_SYSTEM_H

#include <memory>

#include "violet/core/defines.h"
#include "violet/window/window_system.h"

struct SDL_Window;
typedef void * SDL_GLContext;

namespace vlt
{
	struct Deserializer;
	struct Engine;
	struct System_Factory;

	struct VIOLET_API Sdl_Window_System final : public Window_System
	{
		static void install(System_Factory & factory);
		static void init(Deserializer & deserializer);

		Sdl_Window_System(SDL_Window * window, SDL_GLContext context, u32 thread);
		virtual ~Sdl_Window_System() override;

		virtual void render() override;
		virtual bool get_event(Event_Type type, Event* event) override;
		virtual void add_event(Event event) override;
		virtual int get_width() const override;
		virtual int get_height() const override;

	private:

		SDL_Window * m_window;
		SDL_GLContext m_glContext;
		int m_width;
		int m_height;
		u32 m_thread;
	};
}

#endif
