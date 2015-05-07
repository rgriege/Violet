// ============================================================================

#include "engine/input/system/InputSystem.h"

#include "engine/Engine.h"
#include "engine/script/ScriptUtilities.h"
#include "engine/system/SystemFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/window/WindowSystem.h"
#include "engine/input/InputResult.h"

#include <iostream>

using namespace Violet;

// ============================================================================

const char * InputSystem::getStaticLabel()
{
	return "inpt";
}

// ----------------------------------------------------------------------------

void InputSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &InputSystem::init);
}

// ----------------------------------------------------------------------------

std::unique_ptr<System> InputSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	return std::unique_ptr<System>(new InputSystem);
}

// ============================================================================

InputSystem::InputSystem(InputSystem && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

void InputSystem::update(float dt, Engine & engine)
{
	WindowSystem::Event event;
	auto windowSystem = engine.fetch<WindowSystem>();
	while (windowSystem->getEvent(static_cast<WindowSystem::EventType>(WindowSystem::ET_KeyDown | WindowSystem::ET_KeyUp | WindowSystem::ET_MouseDown | WindowSystem::ET_MouseUp | WindowSystem::ET_MouseMove), &event))
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
			onMouseDown(event.mouse.x, event.mouse.y, event.mouse.button, engine);
			break;
		case WindowSystem::ET_MouseUp:
			onMouseUp(event.mouse.x, event.mouse.y, event.mouse.button, engine);
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

// ----------------------------------------------------------------------------

void InputSystem::onMouseMove(int x, int y, int xrel, int yrel, Engine & engine)
{
	auto windowSystem = engine.fetch<WindowSystem>();
	const int width = windowSystem->getWidth();
	const int height = windowSystem->getHeight();
	Vec2f newPoint(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	Vec2f oldPoint(static_cast<float>(x - xrel - width / 2), static_cast<float>(height / 2 - y + yrel));
	for (auto const & componentSet : engine.getCurrentScene().getView<InputComponent, TransformComponent, CppScriptComponent>())
	{
		InputComponent & ic = get<InputComponent&>(componentSet);
		TransformComponent & tc = get<TransformComponent&>(componentSet);
		const bool contains = ic.m_mesh.contains(newPoint - tc.m_position);
		const bool contained = ic.m_mesh.contains(oldPoint - tc.m_position);

		if (contains ^ contained)
		{
			CppScriptComponent & sc = get<CppScriptComponent&>(componentSet);
			if (contains)
				ScriptUtilities::run<void>(sc, "onMouseIn", sc.getEntity(), engine);
			else
				ScriptUtilities::run<void>(sc, "onMouseOut", sc.getEntity(), engine);
		}
	}
}

// ----------------------------------------------------------------------------

void InputSystem::onMouseDown(const int x, const int y, const MouseButton button, Engine & engine)
{
	auto windowSystem = engine.fetch<WindowSystem>();
	const int width = windowSystem->getWidth();
	const int height = windowSystem->getHeight();
	Vec2f point(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	//for (auto it = components.rbegin(), end = components.rend(); it != end; ++it)
	for (auto const & componentSet : engine.getCurrentScene().getView<InputComponent, TransformComponent, CppScriptComponent>())
	{
		InputComponent & ic = get<InputComponent&>(componentSet);
		TransformComponent & tc = get<TransformComponent&>(componentSet);
		CppScriptComponent & sc = get<CppScriptComponent&>(componentSet);
		if (ic.m_mesh.contains(point - tc.m_position) &&
			ScriptUtilities::run<InputResult>(sc, "onMouseDown", ic.getEntity(), engine, std::move(button)) == InputResult::Block)
			break;
	}
}

// ----------------------------------------------------------------------------

void InputSystem::onMouseUp(const int x, const int y, const MouseButton button, Engine & engine)
{
	auto windowSystem = engine.fetch<WindowSystem>();
	const int width = windowSystem->getWidth();
	const int height = windowSystem->getHeight();
	Vec2f point(static_cast<float>(x - width / 2), static_cast<float>(height / 2 - y));
	for (auto const & componentSet : engine.getCurrentScene().getView<InputComponent, TransformComponent, CppScriptComponent>())
	{
		InputComponent & ic = get<InputComponent&>(componentSet);
		TransformComponent & tc = get<TransformComponent&>(componentSet);
		CppScriptComponent & sc = get<CppScriptComponent&>(componentSet);
		if (ic.m_mesh.contains(point - tc.m_position) &&
			ScriptUtilities::run<InputResult>(sc, "onMouseUp", ic.getEntity(), engine, std::move(button)) == InputResult::Block)
			break;
	}
}

// ----------------------------------------------------------------------------

void InputSystem::onKeyDown(const unsigned char key, Engine & engine)
{
	for (auto & componentSet : engine.getCurrentScene().getView<InputComponent, CppScriptComponent>())
	{
		CppScriptComponent & component = get<CppScriptComponent&>(componentSet);
		ScriptUtilities::run<void>(component, "onKeyDown", component.getEntity(), engine, key);
	}
}

// ----------------------------------------------------------------------------

void InputSystem::onKeyUp(const unsigned char key, Engine & engine)
{
	for (auto & componentSet : engine.getCurrentScene().getView<InputComponent, CppScriptComponent>())
	{
		CppScriptComponent & component = get<CppScriptComponent&>(componentSet);
		ScriptUtilities::run<void>(component, "onKeyUp", component.getEntity(), engine, key);
	}
}

// ============================================================================

InputSystem::InputSystem() :
	System(getStaticLabel())
{
}

// ============================================================================
