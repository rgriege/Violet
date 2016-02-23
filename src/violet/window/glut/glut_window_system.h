#ifndef VIOLET_GLUT_WINDOW_SYSTEM_H
#define VIOLET_GLUT_WINDOW_SYSTEM_H

#include <deque>
#include <memory>

#include "violet/math/v2i.h"
#include "violet/window/window_system.h"

namespace GlutWindowSystemNamespace
{
	void onMouseMove(int x, int y);
}

namespace vlt
{
	struct Deserializer;
	struct System_Factory;

	struct VIOLET_API Glut_Window_System final : public Window_System
	{
	public:

		friend void GlutWindowSystemNamespace::onMouseMove(int x, int y);

	public:

		static void install(System_Factory & factory);
		static void init(Deserializer & deserializer);

	public:

		Glut_Window_System(int id, int width, int height);
		virtual ~Glut_Window_System() override = default;

		virtual void update(r32 dt) override;
		virtual void render() override;
		virtual bool get_event(Event_Type type, Event * event) override;
		virtual void add_event(Event event) override;
		virtual int get_width() const override;
		virtual int get_height() const override;

	private:

		const int m_id;
		int m_width;
		int m_height;
		std::deque<Event> m_eventQueue;
		v2i m_mousePos;
	};
}

#endif
