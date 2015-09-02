// ============================================================================

#include "engine/input/system/InputSystem.h"

#include "engine/Engine.h"
#include "engine/input/InputResult.h"
#include "engine/input/component/KeyInputComponent.h"
#include "engine/input/component/MouseInputComponent.h"
#include "engine/math/Matrix3.h"
#include "engine/scene/SceneProcessor.h"
#include "engine/script/ScriptComponent.h"
#include "engine/system/SystemFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/window/WindowSystem.h"

#include <iostream>

using namespace Violet;

namespace InputSystemNamespace
{
	// ----------------------------------------------------------------------------

	void processEvent(const Entity & entity, const WindowSystem::KeyEvent & event, WindowSystem::EventType type, const Engine & engine);
	InputResult processEvent(const Entity & entity, const InputSystem::MouseButtonEvent & event, WindowSystem::EventType type, const Matrix3f & parentToWorld, const Engine & engine);
	InputResult processEvent(const Entity & entity, const InputSystem::MouseMotionEvent & event, const Matrix3f & parentToWorld, const Engine & engine);

	Vec2f computeWorldCoordinates(const Vec2i & windowDimensions, int x, int y);

	// ----------------------------------------------------------------------------
}

using namespace InputSystemNamespace;

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

void InputSystem::update(const float dt, const Engine & engine)
{
	engine.addWriteTask(*engine.getSystem<WindowSystem>(), [&engine](WindowSystem & windowSystem) {
		WindowSystem::Event event;
		const Vec2i windowDimensions(windowSystem.getWidth(), windowSystem.getHeight());

		std::vector<WindowSystem::Event> events;
		while (windowSystem.getEvent(static_cast<WindowSystem::EventType>(~0), &event))
		{
			switch (event.type)
			{
			case WindowSystem::ET_KeyDown:
			case WindowSystem::ET_KeyUp:
				for (auto const & child : engine.getCurrentScene().getRoot().getChildren())
					processEvent(*child, event.key, event.type, engine);
				break;

			case WindowSystem::ET_MouseDown:
			case WindowSystem::ET_MouseUp:
				{
					InputSystem::MouseButtonEvent worldEvent = { computeWorldCoordinates(windowDimensions, event.mouse.x, event.mouse.y), event.mouse.button };
					for (auto const & child : engine.getCurrentScene().getRoot().getChildren())
						processEvent(*child, worldEvent, event.type, Matrix3f::Identity, engine);
				}
				break;

			case WindowSystem::ET_MouseMove:
				{
					InputSystem::MouseMotionEvent worldEvent = { computeWorldCoordinates(windowDimensions, event.motion.x - event.motion.xrel, event.motion.y - event.motion.yrel), computeWorldCoordinates(windowDimensions, event.motion.x, event.motion.y) };
					for (auto const & child : engine.getCurrentScene().getRoot().getChildren())
						processEvent(*child, worldEvent, Matrix3f::Identity, engine);
				}
				break;

			case WindowSystem::ET_Quit:
				engine.addWriteTask(engine, [](Engine & engine) { engine.stop(); });
				break;
			}
		}
	});
}

// ============================================================================

InputSystem::InputSystem() :
	System(getStaticLabel())
{
}

// ============================================================================

void InputSystemNamespace::processEvent(const Entity & entity, const WindowSystem::KeyEvent & event, const WindowSystem::EventType type, const Engine & engine)
{
	for (auto const & child : entity.getChildren())
		processEvent(*child, event, type, engine);

	if (entity.hasComponents<KeyInputComponent, ScriptComponent>())
	{
		if (type == WindowSystem::ET_KeyDown)
		{
			unsigned char code = event.code;
			KeyDownMethod::run(*entity.getComponent<ScriptComponent>()->m_script, entity, engine, std::move(code));
		}
	}
}

// ----------------------------------------------------------------------------

InputResult InputSystemNamespace::processEvent(const Entity & entity, const InputSystem::MouseButtonEvent & event, const WindowSystem::EventType type, const Matrix3f & parentToWorld, const Engine & engine)
{
	if (entity.hasComponents<TransformComponent, MouseInputComponent>())
	{
		auto const & transformComponent = *entity.getComponent<TransformComponent>();
		auto const & mouseComponent = *entity.getComponent<MouseInputComponent>();

		const Matrix3f transform = {
			1.f, 0.f, transformComponent.m_position.x,
			0.f, 1.f, transformComponent.m_position.y,
			0.f, 0.f, 1.f
		};
		const Matrix3f localToWorld = parentToWorld * transform;

		if (entity.hasComponent<ScriptComponent>())
		{
			auto const & script = *entity.getComponent<ScriptComponent>()->m_script;
			if (mouseComponent.m_mesh.getBoundingBox().transform(localToWorld).contains(event.position))
			{
				for (auto const & child : entity.getChildren())
					if (processEvent(*child, event, type, localToWorld, engine) == InputResult::Block)
						return InputResult::Block;

				if (type == WindowSystem::ET_MouseDown)
					MouseDownMethod::run(script, entity, engine, event);
				else
					MouseUpMethod::run(script, entity, engine, event);
			}
		}
	}

	return InputResult::Pass;
}

// ----------------------------------------------------------------------------

InputResult InputSystemNamespace::processEvent(const Entity & entity, const InputSystem::MouseMotionEvent & event, const Matrix3f & parentToWorld, const Engine & engine)
{
	if (entity.hasComponents<TransformComponent, MouseInputComponent>())
	{
		auto const & transformComponent = *entity.getComponent<TransformComponent>();
		auto const & mouseComponent = *entity.getComponent<MouseInputComponent>();

		const Matrix3f transform = {
			1.f, 0.f, transformComponent.m_position.x,
			0.f, 1.f, transformComponent.m_position.y,
			0.f, 0.f, 1.f
		};
		const Matrix3f localToWorld = parentToWorld * transform;
		const AABB worldBoundary = mouseComponent.m_mesh.getBoundingBox().transform(localToWorld);
		const bool contained = worldBoundary.contains(event.from);
		const bool contains = worldBoundary.contains(event.to);

		if (entity.hasComponent<ScriptComponent>())
		{
			auto const & script = *entity.getComponent<ScriptComponent>()->m_script;

			if (contained || contains)
			{
				for (auto const & child : entity.getChildren())
					if (processEvent(*child, event, localToWorld, engine) == InputResult::Block)
						return InputResult::Block;

				if (contained ^ contains)
				{
					if (contains)
						MouseInMethod::run(script, entity, engine);
					else
						MouseOutMethod::run(script, entity, engine);
				}
			}

			MouseMoveMethod::run(script, entity, engine, event);
		}
	}

	return InputResult::Pass;
}

// ----------------------------------------------------------------------------

Vec2f InputSystemNamespace::computeWorldCoordinates(const Vec2i & windowDimensions, const int x, const int y)
{
	return Vec2f(static_cast<float>(x - windowDimensions.x / 2), static_cast<float>(windowDimensions.y / 2 - y));
}

// ============================================================================
