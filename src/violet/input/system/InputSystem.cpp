// ============================================================================

#include "violet/input/system/InputSystem.h"

#include "violet/Engine.h"
#include "violet/input/InputResult.h"
#include "violet/input/component/KeyInputComponent.h"
#include "violet/input/component/MouseInputComponent.h"
#include "violet/math/Matrix3.h"
#include "violet/script/ScriptComponent.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/window/WindowSystem.h"

using namespace Violet;

namespace InputSystemNamespace
{
	// ----------------------------------------------------------------------------

	void processEvent(const ScriptComponent & scriptComponent, const WindowSystem::KeyEvent & event, WindowSystem::EventType type);
	InputResult processEvent(const WorldTransformComponent & tc, const MouseInputComponent & mc, const ScriptComponent & sc, const InputSystem::MouseButtonEvent & event, WindowSystem::EventType type);
	InputResult processEvent(const WorldTransformComponent & tc, const MouseInputComponent & mc, const ScriptComponent & sc, const InputSystem::MouseMotionEvent & event);

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

void InputSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.addSystem(std::unique_ptr<System>(new InputSystem));
		});
}

// ============================================================================

InputSystem::InputSystem(InputSystem && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

void InputSystem::update(const float dt)
{
	const Engine & engine = Engine::getInstance();
	engine.addWriteTask(*engine.getSystem<WindowSystem>(),
		[](WindowSystem & windowSystem)
		{
			const Engine & engine = Engine::getInstance();
			WindowSystem::Event event;
			const Vec2i windowDimensions(windowSystem.getWidth(), windowSystem.getHeight());

			std::vector<WindowSystem::Event> events;
			while (windowSystem.getEvent(static_cast<WindowSystem::EventType>(~0), &event))
			{
				switch (event.type)
				{
				case WindowSystem::ET_KeyDown:
				case WindowSystem::ET_KeyUp:
					for (const auto entity : engine.getCurrentScene().getComponentManager().getEntityView<KeyInputComponent, ScriptComponent>())
						processEvent(std::get<1>(entity), event.key, event.type);
					break;

				case WindowSystem::ET_MouseDown:
				case WindowSystem::ET_MouseUp:
					{
						InputSystem::MouseButtonEvent worldEvent = { computeWorldCoordinates(windowDimensions, event.mouse.x, event.mouse.y), event.mouse.button };
						for (const auto entity : engine.getCurrentScene().getComponentManager().getEntityView<WorldTransformComponent, MouseInputComponent, ScriptComponent>())
							processEvent(std::get<0>(entity), std::get<1>(entity), std::get<2>(entity), worldEvent, event.type);
					}
					break;

				case WindowSystem::ET_MouseMove:
					{
						InputSystem::MouseMotionEvent worldEvent = { computeWorldCoordinates(windowDimensions, event.motion.x - event.motion.xrel, event.motion.y - event.motion.yrel), computeWorldCoordinates(windowDimensions, event.motion.x, event.motion.y) };
						for (const auto entity : engine.getCurrentScene().getComponentManager().getEntityView<WorldTransformComponent, MouseInputComponent, ScriptComponent>())
							processEvent(std::get<0>(entity), std::get<1>(entity), std::get<2>(entity), worldEvent);
					}
					break;

				case WindowSystem::ET_Quit:
					engine.addWriteTask(engine, [](Engine & engine) { engine.stop(); });
					break;
				}
			}
		}, Thread::Window);
}

// ============================================================================

InputSystem::InputSystem() :
	System(getStaticLabel())
{
}

// ============================================================================

void InputSystemNamespace::processEvent(const ScriptComponent & scriptComponent, const WindowSystem::KeyEvent & event, const WindowSystem::EventType type)
{
	if (type == WindowSystem::ET_KeyDown)
	{
		unsigned char code = event.code;
		KeyDownMethod::run(*scriptComponent.m_script, scriptComponent.getEntityId(), std::move(code));
	}
	else if (type == WindowSystem::ET_KeyUp)
	{
		unsigned char code = event.code;
		KeyUpMethod::run(*scriptComponent.m_script, scriptComponent.getEntityId(), std::move(code));
	}
}

// ----------------------------------------------------------------------------

InputResult InputSystemNamespace::processEvent(const WorldTransformComponent & tc, const MouseInputComponent & mc, const ScriptComponent & sc, const InputSystem::MouseButtonEvent & event, const WindowSystem::EventType type)
{
	if (mc.m_mesh.getBoundingBox().transform(tc.m_transform).contains(event.position))
	{
		auto const & script = *sc.m_script;
		if (type == WindowSystem::ET_MouseDown)
			MouseDownMethod::run(script, sc.getEntityId(), event);
		else
			MouseUpMethod::run(script, sc.getEntityId(), event);
	}

	return InputResult::Pass;
}

// ----------------------------------------------------------------------------

InputResult InputSystemNamespace::processEvent(const WorldTransformComponent & tc, const MouseInputComponent & mc, const ScriptComponent & sc, const InputSystem::MouseMotionEvent & event)
{
	const AABB worldBoundary = mc.m_mesh.getBoundingBox().transform(tc.m_transform);
	const bool contained = worldBoundary.contains(event.from);
	const bool contains = worldBoundary.contains(event.to);
	
	auto const & script = *sc.m_script;

	if (contained ^ contains)
	{
		if (contains)
			MouseInMethod::run(script, sc.getEntityId());
		else
			MouseOutMethod::run(script, sc.getEntityId());
	}

	MouseMoveMethod::run(script, sc.getEntityId(), event);

	return InputResult::Pass;
}

// ----------------------------------------------------------------------------

Vec2f InputSystemNamespace::computeWorldCoordinates(const Vec2i & windowDimensions, const int x, const int y)
{
	return Vec2f(static_cast<float>(x - windowDimensions.x / 2), static_cast<float>(windowDimensions.y / 2 - y));
}

// ============================================================================
