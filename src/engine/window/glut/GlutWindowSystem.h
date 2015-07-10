#ifndef VIOLET_GlutWindowSystem_H
#define VIOLET_GlutWindowSystem_H

#include "engine/window/WindowSystem.h"

#include "engine/math/Vec2.h"

#include <memory>
#include <deque>

namespace GlutWindowSystemNamespace
{
	void onMouseMove(int x, int y);
}

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API GlutWindowSystem : public WindowSystem
	{
	public:

		friend void GlutWindowSystemNamespace::onMouseMove(int x, int y);

	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~GlutWindowSystem() override = default;

		virtual void update(float dt, const Engine & engine) override;
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