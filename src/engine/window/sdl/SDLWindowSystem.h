#ifndef VIOLET_SDLWindowSystem_H
#define VIOLET_SDLWindowSystem_H

#include "engine/window/WindowSystem.h"
#include "engine/Defines.h"

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
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~SDLWindowSystem() override;

		virtual void update(float dt, Engine & engine) override;
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