// ============================================================================

#include "violet/input/system/InputSystem.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/input/Key.h"
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
	void processEvent(const WorldTransformComponent & tc, const MouseInputComponent & mc, const ScriptComponent & sc, const InputSystem::MouseMotionEvent & event);

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
		[=](WindowSystem & windowSystem)
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
					if (!m_focussedEntityId.isValid())
					{
						for (const auto entity : engine.getCurrentScene().getEntityView<KeyInputComponent, ScriptComponent>())
							processEvent(std::get<1>(entity), event.key, event.type);
					}
					else
						processFocussedEvent(event.key, event.type);
					break;

				case WindowSystem::ET_MouseDown:
				case WindowSystem::ET_MouseUp:
					{
						InputSystem::MouseButtonEvent worldEvent = { computeWorldCoordinates(windowDimensions, event.mouse.x, event.mouse.y), event.mouse.button, event.mouse.modifiers };
						if (!m_focussedEntityId.isValid())
						{
							for (const auto entity : engine.getCurrentScene().getEntityView<WorldTransformComponent, MouseInputComponent, ScriptComponent>())
								if (processEvent(std::get<0>(entity), std::get<1>(entity), std::get<2>(entity), worldEvent, event.type) == InputResult::Block)
									break;
						}
						else
							processFocussedEvent(worldEvent, event.type);
					}
					break;

				case WindowSystem::ET_MouseMove:
					{
						InputSystem::MouseMotionEvent worldEvent = { computeWorldCoordinates(windowDimensions, event.motion.x - event.motion.xrel, event.motion.y - event.motion.yrel), computeWorldCoordinates(windowDimensions, event.motion.x, event.motion.y) };
						if (!m_focussedEntityId.isValid())
						{
							for (const auto entity : engine.getCurrentScene().getEntityView<WorldTransformComponent, MouseInputComponent, ScriptComponent>())
								processEvent(std::get<0>(entity), std::get<1>(entity), std::get<2>(entity), worldEvent);
						}
						else
							processFocussedEvent(worldEvent);
					}
					break;

				case WindowSystem::ET_Quit:
					QuitEvent::emit(Engine::getInstance());
					engine.addWriteTask(engine, [](Engine & engine) { engine.stop(); });
					break;
				}
			}
		}, Thread::Window);
}

// ----------------------------------------------------------------------------

void InputSystem::focus(const EntityId entityId)
{
	assert(!m_focussedEntityId.isValid());
	if (!m_focussedEntityId.isValid())
		m_focussedEntityId = entityId;
}

// ----------------------------------------------------------------------------

void InputSystem::unfocus(const EntityId entityId)
{
	assert(m_focussedEntityId == entityId);
	if (m_focussedEntityId == entityId)
		m_focussedEntityId = EntityId::ms_invalid;
}

// ============================================================================

InputSystem::InputSystem() :
	System(getStaticLabel()),
	m_focussedEntityId()
{
}

// ----------------------------------------------------------------------------

void InputSystem::processFocussedEvent(const WindowSystem::KeyEvent & event, const WindowSystem::EventType type) thread_const
{
	const auto & engine = Engine::getInstance();
	if (event.code != Key::Escape)
	{
		const auto * sc = engine.getCurrentScene().getComponent<ScriptComponent>(m_focussedEntityId);
		if (sc != nullptr && engine.getCurrentScene().getComponent<KeyInputComponent>(m_focussedEntityId) != nullptr)
			processEvent(*sc, event, type);
		else
			engine.addWriteTask(*this, [=](InputSystem & input) { input.unfocus(m_focussedEntityId); });
	}
	else
	{
		const auto * sc = engine.getCurrentScene().getComponent<ScriptComponent>(m_focussedEntityId);
		if (sc != nullptr)
			FocusLostMethod::run(*sc->m_script, sc->getEntityId());
		engine.addWriteTask(*this, [=](InputSystem & input) { input.unfocus(m_focussedEntityId); });
	}
}

// ----------------------------------------------------------------------------

void InputSystem::processFocussedEvent(const MouseButtonEvent & worldEvent, const WindowSystem::EventType type) thread_const
{
	const auto & engine = Engine::getInstance();
	const auto * sc = engine.getCurrentScene().getComponent<ScriptComponent>(m_focussedEntityId);
	const auto * mc = engine.getCurrentScene().getComponent<MouseInputComponent>(m_focussedEntityId);
	const auto * tc = engine.getCurrentScene().getComponent<WorldTransformComponent>(m_focussedEntityId);
	if (sc && mc && tc)
	{
		auto const & script = *sc->m_script;

		if (mc->m_mesh.getBoundingBox().transform(tc->m_transform).contains(worldEvent.position))
		{
			if (type == WindowSystem::ET_MouseDown)
				MouseDownMethod::run(script, sc->getEntityId(), worldEvent);
			else
				MouseUpMethod::run(script, sc->getEntityId(), worldEvent);
		}
		else
		{
			FocusLostMethod::run(script, sc->getEntityId());
			engine.addWriteTask(*this, [=](InputSystem & input) { input.unfocus(m_focussedEntityId); });
		}
	}
	else
		engine.addWriteTask(*this, [=](InputSystem & input) { input.unfocus(m_focussedEntityId); });
}

// ----------------------------------------------------------------------------

void InputSystem::processFocussedEvent(const MouseMotionEvent & worldEvent) thread_const
{
	const auto & engine = Engine::getInstance();
	const auto * sc = engine.getCurrentScene().getComponent<ScriptComponent>(m_focussedEntityId);
	const auto * mc = engine.getCurrentScene().getComponent<MouseInputComponent>(m_focussedEntityId);
	const auto * tc = engine.getCurrentScene().getComponent<WorldTransformComponent>(m_focussedEntityId);
	if (sc && mc && tc)
		processEvent(*tc, *mc, *sc, worldEvent);
	else
		engine.addWriteTask(*this, [=](InputSystem & input) { input.unfocus(m_focussedEntityId); });
}

// ============================================================================

void InputSystemNamespace::processEvent(const ScriptComponent & scriptComponent, const WindowSystem::KeyEvent & event, const WindowSystem::EventType type)
{
	if (type == WindowSystem::ET_KeyDown)
	{
		unsigned char code = event.code;
		KeyDownMethod::run(*scriptComponent.m_script, scriptComponent.getEntityId(), event);
	}
	else if (type == WindowSystem::ET_KeyUp)
	{
		unsigned char code = event.code;
		KeyUpMethod::run(*scriptComponent.m_script, scriptComponent.getEntityId(), event);
	}
}

// ----------------------------------------------------------------------------

InputResult InputSystemNamespace::processEvent(const WorldTransformComponent & tc, const MouseInputComponent & mc, const ScriptComponent & sc, const InputSystem::MouseButtonEvent & event, const WindowSystem::EventType type)
{
	if (mc.m_mesh.getBoundingBox().transform(tc.m_transform).contains(event.position))
	{
		auto const & script = *sc.m_script;
		return type == WindowSystem::ET_MouseDown
			? MouseDownMethod::run(script, sc.getEntityId(), event)
			: MouseUpMethod::run(script, sc.getEntityId(), event);
	}

	return InputResult::Pass;
}

// ----------------------------------------------------------------------------

void InputSystemNamespace::processEvent(const WorldTransformComponent & tc, const MouseInputComponent & mc, const ScriptComponent & sc, const InputSystem::MouseMotionEvent & event)
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
}

// ----------------------------------------------------------------------------

Vec2f InputSystemNamespace::computeWorldCoordinates(const Vec2i & windowDimensions, const int x, const int y)
{
	return Vec2f(static_cast<float>(x - windowDimensions.x / 2), static_cast<float>(windowDimensions.y / 2 - y));
}

// ============================================================================
