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

	typedef Entity<KeyInputComponent, ScriptComponent> KeyInputEntity;
	void processEvent(const KeyInputEntity & entity, const WindowSystem::KeyEvent & event, WindowSystem::EventType type);

	typedef Entity<WorldTransformComponent, MouseInputComponent, ScriptComponent> MouseInputEntity;
	InputResult processEvent(const MouseInputEntity & entity, const InputSystem::MouseButtonEvent & event, WindowSystem::EventType type);
	void processEvent(const MouseInputEntity & entity, const InputSystem::MouseMotionEvent & event);

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
						for (const auto & entity : engine.getCurrentScene().getEntityView<KeyInputComponent, ScriptComponent>())
							processEvent(entity, event.key, event.type);
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
							for (const auto & entity : engine.getCurrentScene().getEntityView<WorldTransformComponent, MouseInputComponent, ScriptComponent>())
								if (processEvent(entity, worldEvent, event.type) == InputResult::Block)
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
								processEvent(entity, worldEvent);
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
		const auto * kic = engine.getCurrentScene().getComponent<KeyInputComponent>(m_focussedEntityId);
		const auto * sc = engine.getCurrentScene().getComponent<ScriptComponent>(m_focussedEntityId);
		if (sc != nullptr && kic != nullptr)
			processEvent(KeyInputEntity(std::forward_as_tuple(*kic, *sc), m_focussedEntityId), event, type);
		else
			engine.addWriteTask(*this, [=](InputSystem & input) { input.unfocus(m_focussedEntityId); });
	}
	else
	{
		const auto * sc = engine.getCurrentScene().getComponent<ScriptComponent>(m_focussedEntityId);
		if (sc != nullptr)
		{
			EntityId id = m_focussedEntityId;
			FocusLostMethod::run(*sc->m_script, std::move(id));
		}
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
		EntityId id = m_focussedEntityId;

		if (mc->m_mesh.getBoundingBox().transform(to2d(tc->m_transform)).contains(worldEvent.position))
		{
			if (type == WindowSystem::ET_MouseDown)
				MouseDownMethod::run(script, std::move(id), worldEvent);
			else
				MouseUpMethod::run(script, std::move(id), worldEvent);
		}
		else
		{
			FocusLostMethod::run(script, std::move(id));
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
		processEvent(MouseInputEntity(std::forward_as_tuple(*tc, *mc, *sc), m_focussedEntityId), worldEvent);
	else
		engine.addWriteTask(*this, [=](InputSystem & input) { input.unfocus(m_focussedEntityId); });
}

// ============================================================================

void InputSystemNamespace::processEvent(const KeyInputEntity & entity, const WindowSystem::KeyEvent & event, const WindowSystem::EventType type)
{
	if (type == WindowSystem::ET_KeyDown)
	{
		unsigned char code = event.code;
		KeyDownMethod::run(*entity.get<ScriptComponent>().m_script, entity.getId(), event);
	}
	else if (type == WindowSystem::ET_KeyUp)
	{
		unsigned char code = event.code;
		KeyUpMethod::run(*entity.get<ScriptComponent>().m_script, entity.getId(), event);
	}
}

// ----------------------------------------------------------------------------

InputResult InputSystemNamespace::processEvent(const MouseInputEntity & entity, const InputSystem::MouseButtonEvent & event, const WindowSystem::EventType type)
{
	const Matrix3f & mat = to2d(entity.get<WorldTransformComponent>().m_transform);
	const AABB & worldBoundary = entity.get<MouseInputComponent>().m_mesh.getBoundingBox().transform(mat);
	if (worldBoundary.contains(event.position))
	{
		auto const & script = *entity.get<ScriptComponent>().m_script;
		return type == WindowSystem::ET_MouseDown
			? MouseDownMethod::run(script, entity.getId(), event)
			: MouseUpMethod::run(script, entity.getId(), event);
	}

	return InputResult::Pass;
}

// ----------------------------------------------------------------------------

void InputSystemNamespace::processEvent(const MouseInputEntity & entity, const InputSystem::MouseMotionEvent & event)
{
	const Matrix3f & mat = to2d(entity.get<WorldTransformComponent>().m_transform);
	const AABB & worldBoundary = entity.get<MouseInputComponent>().m_mesh.getBoundingBox().transform(mat);
	const bool contained = worldBoundary.contains(event.from);
	const bool contains = worldBoundary.contains(event.to);
	
	auto const & script = *entity.get<ScriptComponent>().m_script;

	if (contained ^ contains)
	{
		if (contains)
			MouseInMethod::run(script, entity.getId());
		else
			MouseOutMethod::run(script, entity.getId());
	}

	MouseMoveMethod::run(script, entity.getId(), event);
}

// ----------------------------------------------------------------------------

Vec2f InputSystemNamespace::computeWorldCoordinates(const Vec2i & windowDimensions, const int x, const int y)
{
	return Vec2f(static_cast<float>(x - windowDimensions.x / 2), static_cast<float>(windowDimensions.y / 2 - y));
}

// ============================================================================
