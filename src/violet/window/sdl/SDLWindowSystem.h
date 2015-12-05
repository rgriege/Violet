#ifndef VIOLET_SDLWindowSystem_H
#define VIOLET_SDLWindowSystem_H

#include "violet/window/WindowSystem.h"
#include "violet/Defines.h"

#include <memory>

struct SDL_Window;
typedef void * SDL_GLContext;

namespace Violet
{
	class Deserializer;
	class Engine;
	class SystemFactory;

	class VIOLET_API SDLWindowSystem : public WindowSystem
	{
	public:

		static void install(SystemFactory & factory);
		static void init(Deserializer & deserializer);

	public:

		virtual ~SDLWindowSystem() override;

		virtual void render() override;
		virtual bool getEvent(EventType type, Event* event) override;
		virtual void addEvent(Event event) override;
		virtual int getWidth() const override;
		virtual int getHeight() const override;

	private:

		SDLWindowSystem(SDL_Window * window, SDL_GLContext context);

	private:

		SDL_Window * m_window;
		SDL_GLContext m_glContext;
		int m_width;
		int m_height;
	};
}

#endif