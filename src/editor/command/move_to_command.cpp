// ============================================================================

#include "editor/command/move_to_command.h"

#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/Log.h"
#include "violet/transform/transform.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/string_utilities.h"

#include <cstdlib>

using namespace edt;
using namespace vlt;

// ============================================================================

const char * move_to_command::get_usage()
{
	return "move <id> <x> <y>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<command> move_to_command::parse(const std::string & text)
{
	std::vector<std::string> arguments;
	string_utilities::split(text, ' ', arguments);
	if (arguments.size() == 3)
	{
		const u32 id = std::atoi(arguments[0].c_str());
		const r32 x = static_cast<r32>(std::atof(arguments[1].c_str()));
		const r32 y = static_cast<r32>(std::atof(arguments[2].c_str()));
		return std::make_unique<move_to_command>(handle(id, ~0), v2(x, y));
	}
	return nullptr;
}

// ============================================================================

move_to_command::move_to_command(const handle _entity_id, const v2 & _position) :
	entity_id(_entity_id),
	position(_position)
{
}

// ----------------------------------------------------------------------------

struct move_to_command_execute_task_data
{
	handle proxy_id;
	v2 position;
};

static void execute_task_local(void * mem)
{
	auto data = make_unique<move_to_command_execute_task_data>(mem);
	auto & proxy_scene = engine::instance().get_current_scene();
	transform::set_position(proxy_scene.get_component<local_transform_component>(data->proxy_id)->transform, data->position);

	// todo: async (can't ready editor_component here)
	const handle proxied_id = proxy_scene.get_component<editor_component>(data->proxy_id)->proxied_id;
	auto * proxied_ltc = engine::instance().get_system<editor_system>()->get_scene().get_component<local_transform_component>(proxied_id);
	transform::set_position(proxied_ltc->transform, data->position);
}

static void execute_task_world(void * mem)
{
	auto data = make_unique<move_to_command_execute_task_data>(mem);
	auto & proxy_scene = engine::instance().get_current_scene();
	transform::set_position(proxy_scene.get_component<world_transform_component>(data->proxy_id)->transform, data->position);

	// todo: async (can't ready editor_component here)
	const handle proxied_id = proxy_scene.get_component<editor_component>(data->proxy_id)->proxied_id;
	auto * proxied_wtc = engine::instance().get_system<editor_system>()->get_scene().get_component<world_transform_component>(proxied_id);
	transform::set_position(proxied_wtc->transform, data->position);
}

void move_to_command::execute()
{
	const auto & proxy_scene = engine::instance().get_current_scene();
	entity_id = handle(entity_id.id, proxy_scene.get_entity_version(entity_id.id));
	log(formatted_string<128>().sprintf("MoveTo::execute %d", entity_id.id));

	const auto * ltc = proxy_scene.get_component<local_transform_component>(entity_id);
	if (ltc != nullptr)
	{
		log(formatted_string<128>().sprintf("MoveTo local <%f,%f>", position.x, position.y));
		add_task(execute_task_local, new move_to_command_execute_task_data{ entity_id, position }, local_transform_component::metadata->thread, task_type::write);
		position = transform::get_position(ltc->transform);
		return;
	}
	
	const auto * wtc = proxy_scene.get_component<world_transform_component>(entity_id);
	if (wtc != nullptr)
	{
		log(formatted_string<128>().sprintf("MoveTo world <%f,%f>", position.x, position.y));
		add_task(execute_task_world, new move_to_command_execute_task_data{ entity_id, position }, world_transform_component::metadata->thread, task_type::write);
		position = transform::get_position(wtc->transform);
		return;
	}
}

// ----------------------------------------------------------------------------

bool move_to_command::can_undo() const
{
	return true;
}

// ----------------------------------------------------------------------------

void move_to_command::undo()
{
	execute();
}

// ============================================================================

