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

namespace Input_System_Namespace
{
	// ----------------------------------------------------------------------------

	typedef Entity<Key_Input_Component, Script_Component> Key_Input_Entity;
	void process_event(const Key_Input_Entity & entity, const Window_System::Key_Event & Event, Window_System::Event_Type type);

	typedef Entity<World_Transform_Component, Mouse_Input_Component, Script_Component> Mouse_Input_Entity;
	Input_Result process_event(const Mouse_Input_Entity & entity, const Input_System::Mouse_Button_Event & Event, Window_System::Event_Type type);
	void process_event(const Mouse_Input_Entity & entity, const Input_System::Mouse_Motion_Event & Event);

	v2 compute_world_coordinates(const v2i & window_dimensions, int x, int y);

	// ----------------------------------------------------------------------------
}

using namespace Input_System_Namespace;

// ============================================================================

const char * Input_System::get_label_static()
{
	return "inpt";
}

// ----------------------------------------------------------------------------

void Input_System::install(System_Factory & factory)
{
	factory.assign(get_label_static(), &Input_System::init);
}

// ----------------------------------------------------------------------------

static void init_task(void*)
{
	Engine::instance().add_system(std::unique_ptr<vlt::System>(new Input_System));
}

void Input_System::init(Deserializer & deserializer)
{
	deserializer.enter_segment(get_label_static());
	add_task(init_task, nullptr, Mouse_Input_Component::metadata->thread, task_type::write);
}

// ============================================================================

Input_System::Input_System() :
	System(get_label_static()),
	focussed_entity_id()
{
}

// ----------------------------------------------------------------------------

Input_System::Input_System(Input_System && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

static void update_task(void*)
{
	Engine & engine = Engine::instance();
	auto & window_system = *engine.get_system<Window_System>();
	const v2i window_dimensions(window_system.get_width(), window_system.get_height());

	const auto & input_system = *engine.get_system<Input_System>();

	Window_System::Event event;
	// todo: make this call const so the task only reads from the Window_System
	while (window_system.get_event(static_cast<Window_System::Event_Type>(~0), &event))
	{
		switch (event.type)
		{
		case Window_System::ET_KeyDown:
		case Window_System::ET_KeyUp:
			if (!input_system.focussed_entity_id.is_valid())
			{
				for (const auto & entity : engine.get_current_scene().get_entity_view<Key_Input_Component, Script_Component>())
					process_event(entity, event.key, event.type);
			}
			else
				input_system.process_focussed_event(event.key, event.type);
			break;

		case Window_System::ET_MouseDown:
		case Window_System::ET_MouseUp:
		{
			Input_System::Mouse_Button_Event world_event = { compute_world_coordinates(window_dimensions, event.mouse.x, event.mouse.y), event.mouse.button, event.mouse.modifiers };
			if (!input_system.focussed_entity_id.is_valid())
			{
				for (const auto & entity : engine.get_current_scene().get_entity_view<World_Transform_Component, Mouse_Input_Component, Script_Component>())
					if (process_event(entity, world_event, event.type) == Input_Result::Block)
						break;
			}
			else
				input_system.process_focussed_event(world_event, event.type);
		}
		break;

		case Window_System::ET_MouseMove:
		{
			Input_System::Mouse_Motion_Event world_event = { compute_world_coordinates(window_dimensions, event.motion.x - event.motion.xrel, event.motion.y - event.motion.yrel), compute_world_coordinates(window_dimensions, event.motion.x, event.motion.y) };
			if (!input_system.focussed_entity_id.is_valid())
			{
				for (const auto entity : engine.get_current_scene().get_entity_view<World_Transform_Component, Mouse_Input_Component, Script_Component>())
					process_event(entity, world_event);
			}
			else
				input_system.process_focussed_event(world_event);
		}
		break;

		case Window_System::ET_Quit:
			QuitEvent::emit(Engine::instance());
			Engine::instance().stop();
			break;
		}
	}
}

void Input_System::update(const r32 /*dt*/)
{
	add_task(update_task, nullptr, Color_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

void Input_System::focus(const Handle entity_id)
{
	assert(!focussed_entity_id.is_valid());
	if (!focussed_entity_id.is_valid())
		focussed_entity_id = entity_id;
}

// ----------------------------------------------------------------------------

void Input_System::unfocus(const Handle entity_id)
{
	assert(focussed_entity_id == entity_id);
	if (focussed_entity_id == entity_id)
		focussed_entity_id = Handle::Invalid;
}

// ============================================================================

struct unfocus_task_data
{
	Handle entity_id;
};

static void unfocus_task(void * mem)
{
	auto data = make_unique<unfocus_task_data>(mem);
	Engine::instance().get_system<Input_System>()->unfocus(data->entity_id);
}

void Input_System::process_focussed_event(const Window_System::Key_Event & event, const Window_System::Event_Type type) thread_const
{
	const auto & engine = Engine::instance();
	if (event.code != Key::Escape)
	{
		const auto * kic = engine.get_current_scene().get_component<Key_Input_Component>(focussed_entity_id);
		const auto * sc = engine.get_current_scene().get_component<Script_Component>(focussed_entity_id);
		if (sc != nullptr && kic != nullptr)
			process_event(Key_Input_Entity(std::forward_as_tuple(*kic, *sc), focussed_entity_id), event, type);
		else
			add_task(unfocus_task, new unfocus_task_data{ focussed_entity_id }, Mouse_Input_Component::metadata->thread, task_type::write);
	}
	else
	{
		const auto * sc = engine.get_current_scene().get_component<Script_Component>(focussed_entity_id);
		if (sc != nullptr)
		{
			Handle id = focussed_entity_id;
			FocusLostMethod::run(*sc->script, std::move(id));
		}
		add_task(unfocus_task, new unfocus_task_data{ focussed_entity_id }, Mouse_Input_Component::metadata->thread, task_type::write);
	}
}

// ----------------------------------------------------------------------------

void Input_System::process_focussed_event(const Mouse_Button_Event & world_event, const Window_System::Event_Type type) thread_const
{
	const auto & engine = Engine::instance();
	const auto * sc = engine.get_current_scene().get_component<Script_Component>(focussed_entity_id);
	const auto * mc = engine.get_current_scene().get_component<Mouse_Input_Component>(focussed_entity_id);
	const auto * tc = engine.get_current_scene().get_component<World_Transform_Component>(focussed_entity_id);
	if (sc && mc && tc)
	{
		auto const & script = *sc->script;
		Handle id = focussed_entity_id;

		if (mc->mesh.get_bounding_box().transform(to2d(tc->transform)).contains(world_event.position))
		{
			if (type == Window_System::ET_MouseDown)
				MouseDownMethod::run(script, std::move(id), world_event);
			else
				MouseUpMethod::run(script, std::move(id), world_event);
		}
		else
		{
			FocusLostMethod::run(script, std::move(id));
			add_task(unfocus_task, new unfocus_task_data{ focussed_entity_id }, Mouse_Input_Component::metadata->thread, task_type::write);
		}
	}
	else
		add_task(unfocus_task, new unfocus_task_data{ focussed_entity_id }, Mouse_Input_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

void Input_System::process_focussed_event(const Mouse_Motion_Event & world_event) thread_const
{
	const auto & engine = Engine::instance();
	const auto * sc = engine.get_current_scene().get_component<Script_Component>(focussed_entity_id);
	const auto * mc = engine.get_current_scene().get_component<Mouse_Input_Component>(focussed_entity_id);
	const auto * tc = engine.get_current_scene().get_component<World_Transform_Component>(focussed_entity_id);
	if (sc && mc && tc)
		process_event(Mouse_Input_Entity(std::forward_as_tuple(*tc, *mc, *sc), focussed_entity_id), world_event);
	else
		add_task(unfocus_task, new unfocus_task_data{ focussed_entity_id }, Mouse_Input_Component::metadata->thread, task_type::write);
}

// ============================================================================

void Input_System_Namespace::process_event(const Key_Input_Entity & entity, const Window_System::Key_Event & event, const Window_System::Event_Type type)
{
	Handle id = entity.id;
	if (type == Window_System::ET_KeyDown)
	{
		unsigned char code = event.code;
		KeyDownMethod::run(*entity.get<Script_Component>().script, std::move(id), event);
	}
	else if (type == Window_System::ET_KeyUp)
	{
		unsigned char code = event.code;
		KeyUpMethod::run(*entity.get<Script_Component>().script, std::move(id), event);
	}
}

// ----------------------------------------------------------------------------

Input_Result Input_System_Namespace::process_event(const Mouse_Input_Entity & entity, const Input_System::Mouse_Button_Event & event, const Window_System::Event_Type type)
{
	const m3 & mat = to2d(entity.get<World_Transform_Component>().transform);
	const aabb & world_boundary = entity.get<Mouse_Input_Component>().mesh.get_bounding_box().transform(mat);
	if (world_boundary.contains(event.position))
	{
		Handle id = entity.id;
		auto const & script = *entity.get<Script_Component>().script;
		return type == Window_System::ET_MouseDown
			? MouseDownMethod::run(script, std::move(id), event)
			: MouseUpMethod::run(script, std::move(id), event);
	}

	return Input_Result::Pass;
}

// ----------------------------------------------------------------------------

void Input_System_Namespace::process_event(const Mouse_Input_Entity & entity, const Input_System::Mouse_Motion_Event & event)
{
	const m3 & mat = to2d(entity.get<World_Transform_Component>().transform);
	const aabb & world_boundary = entity.get<Mouse_Input_Component>().mesh.get_bounding_box().transform(mat);
	const bool contained = world_boundary.contains(event.from);
	const bool contains = world_boundary.contains(event.to);

	Handle id = entity.id;
	auto const & script = *entity.get<Script_Component>().script;

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

v2 Input_System_Namespace::compute_world_coordinates(const v2i & window_dimensions, const int x, const int y)
{
	return v2(static_cast<r32>(x - window_dimensions.x / 2), static_cast<r32>(window_dimensions.y / 2 - y));
}

// ============================================================================
