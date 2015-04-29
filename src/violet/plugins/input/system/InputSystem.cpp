#include "violet/plugins/input/system/InputSystem.h"

#include "violet/core/Engine.h"
#include "violet/core/script/ScriptUtilities.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/core/window/WindowSystem.h"

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

InputSystem::InputSystem(InputSystem && other) :
	ComponentSystem<InputComponent>(std::move(other))
{
}

void InputSystem::update(float /*dt*/, Engine & engine)
{
	WindowSystem::Event event;
	auto & windowSystem = engine.fetch<WindowSystem>();
	while (windowSystem.getEvent(static_cast<WindowSystem::EventType>(WindowSystem::ET_KeyDown | WindowSystem::ET_KeyUp | WindowSystem::ET_MouseDown | WindowSystem::ET_MouseUp | WindowSystem::ET_MouseMove), &event))
	{
		switch (event.type)
		{
		case WindowSystem::ET_KeyDown:
			onKeyDown(event.key.code, engine);
			break;
		case WindowSystem::ET_KeyUp:
			onKeyUp(event.key.code, engine);
			break;
		case WindowSystem::ET_MouseDown:
			onMouseDown(event.mouse.x, event.mouse.y, engine);
			break;
		case WindowSystem::ET_MouseUp:
			onMouseUp(event.mouse.x, event.mouse.y, engine);
			break;
		case WindowSystem::ET_MouseMove:
			onMouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel, engine);
			break;
		case WindowSystem::ET_Quit:
			engine.stop();
			break;
		}
	}
}

void InputSystem::onMouseMove(int x, int y, int xrel, int yrel, Engine & engine)
{
	auto & windowSystem = engine.fetch<WindowSystem>();
	const int width = windowSystem.getWidth();
	const int height = windowSystem.getHeight();
	Vec2f newPoint(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	Vec2f oldPoint(static_cast<float>(x - xrel - width / 2), static_cast<float>(height / 2 - y + yrel));
	for (auto const & component : getComponents())
	{
		auto const & transform = engine.fetch<TransformComponent>(component.getEntity());
		const bool contains = component.m_mesh.contains(newPoint - transform.m_position);
		const bool contained = component.m_mesh.contains(oldPoint - transform.m_position);

		if (contains ^ contained)
		{
			if (contains)
				ScriptUtilities::run<void>(engine, component.getEntity(), "onMouseIn", component.getEntity(), engine);
			else
				ScriptUtilities::run<void>(engine, component.getEntity(), "onMouseOut", component.getEntity(), engine);
		}
	}
}

void InputSystem::onMouseDown(const int x, const int y, Engine & engine)
{
	auto & windowSystem = engine.fetch<WindowSystem>();
	const int width = windowSystem.getWidth();
	const int height = windowSystem.getHeight();
	Vec2f point(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	for (auto const & component : getComponents())
	{
		auto const & transform = engine.fetch<TransformComponent>(component.getEntity());
		if (component.m_mesh.contains(point - transform.m_position))
			ScriptUtilities::run<void>(engine, component.getEntity(), "onMouseDown", component.getEntity(), engine);
	}
}

void InputSystem::onMouseUp(const int x, const int y, Engine & engine)
{
	auto & windowSystem = engine.fetch<WindowSystem>();
	const int width = windowSystem.getWidth();
	const int height = windowSystem.getHeight();
	Vec2f point(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	for (auto const & component : getComponents())
	{
		auto const & transform = engine.fetch<TransformComponent>(component.getEntity());
		if (component.m_mesh.contains(point - transform.m_position))
			ScriptUtilities::run<void>(engine, component.getEntity(), "onMouseUp", component.getEntity(), engine);
	}
}

void InputSystem::onKeyDown(const unsigned char key, Engine & engine)
{
	for (auto const & component : getComponents())
		ScriptUtilities::run<void>(engine, component.getEntity(), "onKeyDown", component.getEntity(), engine, key);
}

void InputSystem::onKeyUp(const unsigned char key, Engine & engine)
{
	for (auto const & component : getComponents())
		ScriptUtilities::run<void>(engine, component.getEntity(), "onKeyDown", component.getEntity(), engine, key);
}
