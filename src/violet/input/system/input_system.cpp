// ============================================================================

#include "violet/input/system/input_system.h"

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/color_component.h"
#include "violet/input/key.h"
#include "violet/input/input_result.h"
#include "violet/input/component/key_input_component.h"
#include "violet/input/component/mouse_input_component.h"
#include "violet/math/m3.h"
#include "violet/math/v2i.h"
#include "violet/script/script_component.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/memory.h"
#include "violet/window/window_system.h"

using namespace vlt;

namespace input_systemNamespace
{
	// ----------------------------------------------------------------------------

	typedef entity<key_input_component, script_component> KeyInputEntity;
	void process_event(const KeyInputEntity & entity, const window_system::key_event & event, window_system::event_type type);

	typedef entity<world_transform_component, mouse_input_component, script_component> MouseInputEntity;
	input_result process_event(const MouseInputEntity & entity, const input_system::MouseButtonEvent & event, window_system::event_type type);
	void process_event(const MouseInputEntity & entity, const input_system::MouseMotionEvent & event);

	v2 computeWorldCoordinates(const v2i & windowDimensions, int x, int y);

	// ----------------------------------------------------------------------------
}

using namespace input_systemNamespace;

// ============================================================================

const char * input_system::get_label_static()
{
	return "inpt";
}

// ----------------------------------------------------------------------------

void input_system::install(system_factory & factory)
{
	factory.assign(get_label_static(), &input_system::init);
}

// ----------------------------------------------------------------------------

static void init_task(void*)
{
	engine::instance().add_system(std::unique_ptr<vlt::system>(new input_system));
}

void input_system::init(deserializer & deserializer)
{
	deserializer.enter_segment(get_label_static());
	add_task(init_task, nullptr, mouse_input_component::metadata->thread, task_type::write);
}

// ============================================================================

input_system::input_system() :
	system(get_label_static()),
	m_focussedEntityId()
{
}

// ----------------------------------------------------------------------------

input_system::input_system(input_system && other) :
	system(std::move(other))
{
}

// ----------------------------------------------------------------------------

static void update_task(void*)
{
	engine & e = engine::instance();
	auto & windowSystem = *e.get_system<window_system>();
	const v2i windowDimensions(windowSystem.get_width(), windowSystem.get_height());

	const auto & inputSystem = *e.get_system<input_system>();

	window_system::event event;
	// todo: make this call const so the task only reads from the window_system
	while (windowSystem.get_event(static_cast<window_system::event_type>(~0), &event))
	{
		switch (event.type)
		{
		case window_system::ET_KeyDown:
		case window_system::ET_KeyUp:
			if (!inputSystem.m_focussedEntityId.is_valid())
			{
				for (const auto & entity : e.get_current_scene().get_entity_view<key_input_component, script_component>())
					process_event(entity, event.key, event.type);
			}
			else
				inputSystem.process_focussed_event(event.key, event.type);
			break;

		case window_system::ET_MouseDown:
		case window_system::ET_MouseUp:
		{
			input_system::MouseButtonEvent worldEvent = { computeWorldCoordinates(windowDimensions, event.mouse.x, event.mouse.y), event.mouse.button, event.mouse.modifiers };
			if (!inputSystem.m_focussedEntityId.is_valid())
			{
				for (const auto & entity : e.get_current_scene().get_entity_view<world_transform_component, mouse_input_component, script_component>())
					if (process_event(entity, worldEvent, event.type) == input_result::block)
						break;
			}
			else
				inputSystem.process_focussed_event(worldEvent, event.type);
		}
		break;

		case window_system::ET_MouseMove:
		{
			input_system::MouseMotionEvent worldEvent = { computeWorldCoordinates(windowDimensions, event.motion.x - event.motion.xrel, event.motion.y - event.motion.yrel), computeWorldCoordinates(windowDimensions, event.motion.x, event.motion.y) };
			if (!inputSystem.m_focussedEntityId.is_valid())
			{
				for (const auto entity : e.get_current_scene().get_entity_view<world_transform_component, mouse_input_component, script_component>())
					process_event(entity, worldEvent);
			}
			else
				inputSystem.process_focussed_event(worldEvent);
		}
		break;

		case window_system::ET_Quit:
			QuitEvent::emit(engine::instance());
			engine::instance().stop();
			break;
		}
	}
}

void input_system::update(const r32 /*dt*/)
{
	add_task(update_task, nullptr, color_component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

void input_system::focus(const handle entity_id)
{
	assert(!m_focussedEntityId.is_valid());
	if (!m_focussedEntityId.is_valid())
		m_focussedEntityId = entity_id;
}

// ----------------------------------------------------------------------------

void input_system::unfocus(const handle entity_id)
{
	assert(m_focussedEntityId == entity_id);
	if (m_focussedEntityId == entity_id)
		m_focussedEntityId = handle::Invalid;
}

// ============================================================================

struct unfocus_task_data
{
	handle entity_id;
};

static void unfocus_task(void * mem)
{
	auto data = make_unique<unfocus_task_data>(mem);
	engine::instance().get_system<input_system>()->unfocus(data->entity_id);
}

void input_system::process_focussed_event(const window_system::key_event & event, const window_system::event_type type) thread_const
{
	const auto & engine = engine::instance();
	if (event.code != key::Escape)
	{
		const auto * kic = engine.get_current_scene().get_component<key_input_component>(m_focussedEntityId);
		const auto * sc = engine.get_current_scene().get_component<script_component>(m_focussedEntityId);
		if (sc != nullptr && kic != nullptr)
			process_event(KeyInputEntity(std::forward_as_tuple(*kic, *sc), m_focussedEntityId), event, type);
		else
			add_task(unfocus_task, new unfocus_task_data{ m_focussedEntityId }, mouse_input_component::metadata->thread, task_type::write);
	}
	else
	{
		const auto * sc = engine.get_current_scene().get_component<script_component>(m_focussedEntityId);
		if (sc != nullptr)
		{
			handle id = m_focussedEntityId;
			FocusLostMethod::run(*sc->script, std::move(id));
		}
		add_task(unfocus_task, new unfocus_task_data{ m_focussedEntityId }, mouse_input_component::metadata->thread, task_type::write);
	}
}

// ----------------------------------------------------------------------------

void input_system::process_focussed_event(const MouseButtonEvent & worldEvent, const window_system::event_type type) thread_const
{
	const auto & engine = engine::instance();
	const auto * sc = engine.get_current_scene().get_component<script_component>(m_focussedEntityId);
	const auto * mc = engine.get_current_scene().get_component<mouse_input_component>(m_focussedEntityId);
	const auto * tc = engine.get_current_scene().get_component<world_transform_component>(m_focussedEntityId);
	if (sc && mc && tc)
	{
		auto const & script = *sc->script;
		handle id = m_focussedEntityId;

		if (mc->m_mesh.get_bounding_box().transform(to2d(tc->transform)).contains(worldEvent.position))
		{
			if (type == window_system::ET_MouseDown)
				MouseDownMethod::run(script, std::move(id), worldEvent);
			else
				MouseUpMethod::run(script, std::move(id), worldEvent);
		}
		else
		{
			FocusLostMethod::run(script, std::move(id));
			add_task(unfocus_task, new unfocus_task_data{ m_focussedEntityId }, mouse_input_component::metadata->thread, task_type::write);
		}
	}
	else
		add_task(unfocus_task, new unfocus_task_data{ m_focussedEntityId }, mouse_input_component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

void input_system::process_focussed_event(const MouseMotionEvent & worldEvent) thread_const
{
	const auto & engine = engine::instance();
	const auto * sc = engine.get_current_scene().get_component<script_component>(m_focussedEntityId);
	const auto * mc = engine.get_current_scene().get_component<mouse_input_component>(m_focussedEntityId);
	const auto * tc = engine.get_current_scene().get_component<world_transform_component>(m_focussedEntityId);
	if (sc && mc && tc)
		process_event(MouseInputEntity(std::forward_as_tuple(*tc, *mc, *sc), m_focussedEntityId), worldEvent);
	else
		add_task(unfocus_task, new unfocus_task_data{ m_focussedEntityId }, mouse_input_component::metadata->thread, task_type::write);
}

// ============================================================================

void input_systemNamespace::process_event(const KeyInputEntity & entity, const window_system::key_event & event, const window_system::event_type type)
{
	handle id = entity.id;
	if (type == window_system::ET_KeyDown)
	{
		unsigned char code = event.code;
		KeyDownMethod::run(*entity.get<script_component>().script, std::move(id), event);
	}
	else if (type == window_system::ET_KeyUp)
	{
		unsigned char code = event.code;
		KeyUpMethod::run(*entity.get<script_component>().script, std::move(id), event);
	}
}

// ----------------------------------------------------------------------------

input_result input_systemNamespace::process_event(const MouseInputEntity & entity, const input_system::MouseButtonEvent & event, const window_system::event_type type)
{
	const m3 & mat = to2d(entity.get<world_transform_component>().transform);
	const aabb & worldBoundary = entity.get<mouse_input_component>().m_mesh.get_bounding_box().transform(mat);
	if (worldBoundary.contains(event.position))
	{
		handle id = entity.id;
		auto const & script = *entity.get<script_component>().script;
		return type == window_system::ET_MouseDown
			? MouseDownMethod::run(script, std::move(id), event)
			: MouseUpMethod::run(script, std::move(id), event);
	}

	return input_result::Pass;
}

// ----------------------------------------------------------------------------

void input_systemNamespace::process_event(const MouseInputEntity & entity, const input_system::MouseMotionEvent & event)
{
	const m3 & mat = to2d(entity.get<world_transform_component>().transform);
	const aabb & worldBoundary = entity.get<mouse_input_component>().m_mesh.get_bounding_box().transform(mat);
	const bool contained = worldBoundary.contains(event.from);
	const bool contains = worldBoundary.contains(event.to);

	handle id = entity.id;
	auto const & script = *entity.get<script_component>().script;

	if (contained ^ contains)
	{
		if (contains)
			MouseInMethod::run(script, std::move(id));
		else
			MouseOutMethod::run(script, std::move(id));
	}

	MouseMoveMethod::run(script, std::move(id), event);
}

// ----------------------------------------------------------------------------

v2 input_systemNamespace::computeWorldCoordinates(const v2i & windowDimensions, const int x, const int y)
{
	return v2(static_cast<r32>(x - windowDimensions.x / 2), static_cast<r32>(windowDimensions.y / 2 - y));
}

// ============================================================================
