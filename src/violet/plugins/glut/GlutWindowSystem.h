#ifndef GLUT_WINDOW_H
#define GLUT_WINDOW_H

#include "violet/core/window/WindowSystem.h"

#include "violet/core/math/Vec2.h"

#include <memory>
#include <deque>

#ifdef WIN32
#ifdef VIOLETGLUT_EXPORT
#define VIOLET_GLUT_API __declspec(dllexport)
#else
#define VIOLET_GLUT_API __declspec(dllimport)
#endif
#else
#define VIOLET_API
#endif

namespace GlutWindowSystemNamespace
{
	void onMouseMove(int x, int y);
}

namespace Violet
{
	class Deserializer;

	class VIOLET_GLUT_API GlutWindowSystem : public WindowSystem
	{
	public:

		friend void GlutWindowSystemNamespace::onMouseMove(int x, int y);

	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~GlutWindowSystem() override = default;

		virtual void update(float dt, Engine & engine) override;
		virtual void render() override;
		virtual bool getEvent(EventType type, Event * event) override;
		virtual void addEvent(Event event) override;
		virtual int getWidth() const override;
		virtual int getHeight() const override;

	private:

		GlutWindowSystem(int id, int width, int height);

	private:

		const int m_id;
		int m_width;
		int m_height;
		std::deque<Event> m_eventQueue;
		Vec2i m_mousePos;
	};
}

#endif