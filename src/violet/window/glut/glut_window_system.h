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
	struct deserializer;
	struct system_factory;

	struct VIOLET_API glut_window_system final : public window_system
	{
	public:

		friend void GlutWindowSystemNamespace::onMouseMove(int x, int y);

	public:

		static void install(system_factory & factory);
		static void init(deserializer & deserializer);

	public:

		glut_window_system(int id, int width, int height);
		virtual ~glut_window_system() override = default;

		virtual void update(r32 dt) override;
		virtual void render() override;
		virtual bool get_event(event_type type, event * event) override;
		virtual void add_event(event event) override;
		virtual int get_width() const override;
		virtual int get_height() const override;

	private:

		const int m_id;
		int m_width;
		int m_height;
		std::deque<event> m_eventQueue;
		v2i m_mousePos;
	};
}

#endif