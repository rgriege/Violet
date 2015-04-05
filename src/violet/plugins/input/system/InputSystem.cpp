#include "violet/plugins/input/system/InputSystem.h"

#include "violet/core/Engine.h"
#include "violet/core/script/Procedure.h"
#include "violet/core/script/system/ScriptSystem.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/core/window/Window.h"

using namespace Violet;

void InputSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &InputSystem::init);
}

std::unique_ptr<System> InputSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	return std::unique_ptr<System>(new InputSystem);
}

void InputSystem::update(float /*dt*/, Engine & engine)
{
	Window::Event event;
	while (Window::getCurrent().getEvent(static_cast<Window::EventType>(Window::ET_KeyDown | Window::ET_KeyUp | Window::ET_MouseDown | Window::ET_MouseUp | Window::ET_MouseMove), &event))
	{
		switch (event.type)
		{
		case Window::ET_KeyDown:
			onKeyDown(event.key.code, engine);
			break;
		case Window::ET_KeyUp:
			onKeyUp(event.key.code, engine);
			break;
		case Window::ET_MouseDown:
			onMouseDown(event.mouse.x, event.mouse.y, engine);
			break;
		case Window::ET_MouseUp:
			onMouseUp(event.mouse.x, event.mouse.y, engine);
			break;
		case Window::ET_MouseMove:
			onMouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel, engine);
			break;
		}
	}
}

void InputSystem::onMouseMove(int x, int y, int xrel, int yrel, Engine & engine)
{
	const int width = Window::getCurrent().getWidth();
	const int height = Window::getCurrent().getHeight();
	Vec2f newPoint(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	Vec2f oldPoint(static_cast<float>(x - xrel - width / 2), static_cast<float>(height / 2 - y + yrel));
	for (auto const & component : getComponents())
	{
		auto const & transform = engine.fetch<TransformComponent>(component.getEntity());
		const bool contains = component.m_mesh.contains(newPoint - transform.m_position);
		const bool contained = component.m_mesh.contains(oldPoint - transform.m_position);

		if (contains ^ contained)
		{
			auto const & scriptComponent = engine.fetch<ScriptComponent>(component.getEntity());
			if (contains)
				scriptComponent.m_script->run(Procedure::create("onMouseIn", component.getEntity(), engine));
			else
				scriptComponent.m_script->run(Procedure::create("onMouseOut", component.getEntity(), engine));
		}
	}
}

void InputSystem::onMouseDown(const int x, const int y, Engine & engine)
{
	const int width = Window::getCurrent().getWidth();
	const int height = Window::getCurrent().getHeight();
	Vec2f point(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	for (auto const & component : getComponents())
	{
		auto const & transform = engine.fetch<TransformComponent>(component.getEntity());
		if (component.m_mesh.contains(point - transform.m_position))
		{
			auto const & scriptComponent = engine.fetch<ScriptComponent>(component.getEntity());
			scriptComponent.m_script->run(Procedure::create("onMouseDown", component.getEntity(), engine));
		}
	}
}

void InputSystem::onMouseUp(const int x, const int y, Engine & engine)
{
	const int width = Window::getCurrent().getWidth();
	const int height = Window::getCurrent().getHeight();
	Vec2f point(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	for (auto const & component : getComponents())
	{
		auto const & transform = engine.fetch<TransformComponent>(component.getEntity());
		if (component.m_mesh.contains(point - transform.m_position))
		{
			auto const & scriptComponent = engine.fetch<ScriptComponent>(component.getEntity());
			scriptComponent.m_script->run(Procedure::create("onMouseUp", component.getEntity(), engine));
		}
	}
}

/*class KeyboardDownEvent
{
public:

	typedef std::function<void(const Entity & entity, AlterContext & context, unsigned char key)> Listener;

public:

	static void subscribe(const Entity & entity, Listener listener)
	{
		m_listeners[entity] = listener;
	}

	static void send(const Entity & entity, AlterContext & context, unsigned char key)
	{
		auto it = m_listeners.find(entity);
		if (it != m_listeners.end())
			it->second(entity, context, key);
	}

private:

	std::map<Entity, Listener> m_listeners;
};*/

void InputSystem::onKeyDown(const unsigned char key, Engine & engine)
{
	for (auto const & component : getComponents())
	{
		auto const & scriptComponent = engine.fetch<ScriptComponent>(component.getEntity());
		scriptComponent.m_script->run(Procedure::create("onKeyDown", component.getEntity(), engine, key));
	}
}

void InputSystem::onKeyUp(const unsigned char key, Engine & engine)
{
	static int i = 0;
	for (auto const & component : getComponents())
	{
		auto const & scriptComponent = engine.fetch<ScriptComponent>(component.getEntity());
		scriptComponent.m_script->run(Procedure::create("onKeyUp", component.getEntity(), engine, key));
	}
}
