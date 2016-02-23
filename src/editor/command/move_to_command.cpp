// ============================================================================

#include <cstdlib>

#include "editor/command/move_to_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/log.h"
#include "violet/transform/transform.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * Move_To_Command::get_usage()
{
	return "move <id> <x> <y>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> Move_To_Command::parse(const std::string & text)
{
	std::vector<std::string> arguments;
	String_Utilities::split(text, ' ', arguments);
	if (arguments.size() == 3)
	{
		const u32 id = std::atoi(arguments[0].c_str());
		const r32 x = static_cast<r32>(std::atof(arguments[1].c_str()));
		const r32 y = static_cast<r32>(std::atof(arguments[2].c_str()));
		return std::make_unique<Move_To_Command>(Handle(id, ~0), v2(x, y));
	}
	return nullptr;
}

// ============================================================================

Move_To_Command::Move_To_Command(const Handle _entity_id, const v2 & _position) :
	entity_id(_entity_id),
	position(_position)
{
}

// ----------------------------------------------------------------------------

struct move_to_command_execute_task_data
{
	Handle proxy_id;
	v2 position;
};

static void execute_task_local(void * mem)
{
	auto data = make_unique<move_to_command_execute_task_data>(mem);
	auto & proxy_scene = Engine::instance().get_current_scene();
	Transform::set_position(proxy_scene.get_component<Local_Transform_Component>(data->proxy_id)->transform, data->position);

	// todo: async (can't ready Editor_Component here)
	const Handle proxied_id = proxy_scene.get_component<Editor_Component>(data->proxy_id)->proxied_id;
	auto * proxied_ltc = Engine::instance().get_system<Editor_System>()->get_scene().get_component<Local_Transform_Component>(proxied_id);
	Transform::set_position(proxied_ltc->transform, data->position);
}

static void execute_task_world(void * mem)
{
	auto data = make_unique<move_to_command_execute_task_data>(mem);
	auto & proxy_scene = Engine::instance().get_current_scene();
	Transform::set_position(proxy_scene.get_component<World_Transform_Component>(data->proxy_id)->transform, data->position);

	// todo: async (can't ready Editor_Component here)
	const Handle proxied_id = proxy_scene.get_component<Editor_Component>(data->proxy_id)->proxied_id;
	auto * proxied_wtc = Engine::instance().get_system<Editor_System>()->get_scene().get_component<World_Transform_Component>(proxied_id);
	Transform::set_position(proxied_wtc->transform, data->position);
}

void Move_To_Command::execute()
{
	const auto & proxy_scene = Engine::instance().get_current_scene();
	entity_id = Handle(entity_id.id, proxy_scene.get_entity_version(entity_id.id));
	log(Formatted_String<128>().sprintf("MoveTo::execute %d", entity_id.id));

	const auto * ltc = proxy_scene.get_component<Local_Transform_Component>(entity_id);
	if (ltc != nullptr)
	{
		log(Formatted_String<128>().sprintf("MoveTo local <%f,%f>", position.x, position.y));
		add_task(execute_task_local, new move_to_command_execute_task_data{ entity_id, position }, Local_Transform_Component::metadata->thread, task_type::write);
		position = Transform::get_position(ltc->transform);
		return;
	}
	
	const auto * wtc = proxy_scene.get_component<World_Transform_Component>(entity_id);
	if (wtc != nullptr)
	{
		log(Formatted_String<128>().sprintf("MoveTo world <%f,%f>", position.x, position.y));
		add_task(execute_task_world, new move_to_command_execute_task_data{ entity_id, position }, World_Transform_Component::metadata->thread, task_type::write);
		position = Transform::get_position(wtc->transform);
		return;
	}
}

// ----------------------------------------------------------------------------

bool Move_To_Command::can_undo() const
{
	return true;
}

// ----------------------------------------------------------------------------

void Move_To_Command::undo()
{
	execute();
}

// ============================================================================

