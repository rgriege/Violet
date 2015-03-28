#ifndef GLUT_WINDOW_H
#define GLUT_WINDOW_H

#include "violet/core/window/Window.h"

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

namespace Violet
{
	class Deserializer;

	class VIOLET_GLUT_API GlutWindow : public Window
	{
	public:

		static std::unique_ptr<Window> create(Deserializer & deserializer);

	public:

		virtual ~GlutWindow() override = default;

		virtual void update() override;
		virtual void render() override;
		virtual bool getEvent(EventType type, Event * event) override;
		virtual void addEvent(Event event) override;
		virtual int getWidth() const override;
		virtual int getHeight() const override;

	private:

		GlutWindow(int width, int height);

	private:

		int m_width;
		int m_height;
		std::deque<Event> m_eventQueue;
	};
}

#endif