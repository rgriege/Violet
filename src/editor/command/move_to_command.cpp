// ============================================================================

#include "editor/command/move_to_command.h"

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

move_to_command::move_to_command(const handle entity_id, const v2 & position) :
	m_entityId(entity_id),
	m_position(position)
{
}

// ----------------------------------------------------------------------------

struct execute_task_data
{
	handle entity_id;
	v2 position;
};

static void execute_task_local(void * mem)
{
	auto data = make_unique<execute_task_data>(mem);
	auto & editor = *engine::instance().get_system<editor_system>();
	auto ltc = editor.get_scene().get_component<local_transform_component>(data->entity_id);
	if (ltc != nullptr)
	{
		transform::set_position(ltc->transform, data->position);
		const handle proxy_id = editor.get_proxy_id(data->entity_id);
		engine::instance().get_current_scene().get_component<local_transform_component>(proxy_id)->transform = ltc->transform;
	}
}

static void execute_task_world(void * mem)
{
	auto data = make_unique<execute_task_data>(mem);
	auto & editor = *engine::instance().get_system<editor_system>();
	auto wtc = editor.get_scene().get_component<world_transform_component>(data->entity_id);
	if (wtc != nullptr)
	{
		transform::set_position(wtc->transform, data->position);
		// const handle proxy_id = editor.get_proxy_id(data->entity_id);
		// engine::instance().get_current_scene().get_component<world_transform_component>(proxy_id)->transform = ltc->transform;
	}
}

void move_to_command::execute()
{
	log(formatted_string<128>().sprintf("MoveTo::execute %d", m_entityId.id));
	const auto & engine = engine::instance();
	const auto & editor = *engine.get_system<editor_system>();
	const auto & scene = editor.get_scene();
	if (m_entityId.version == handle::Invalid.version)
		m_entityId = handle(m_entityId.id, scene.get_entity_version(m_entityId.id));

	const handle entity_id = m_entityId;
	const v2 newPosition = m_position;

	const auto * ltc = scene.get_component<local_transform_component>(m_entityId);
	if (ltc != nullptr)
	{
		log(formatted_string<128>().sprintf("MoveTo local <%f,%f>", m_position.x, m_position.y));
		add_task(execute_task_local, new execute_task_data{ entity_id, m_position }, local_transform_component::metadata->thread, task_type::write);
		m_position = transform::get_position(ltc->transform);
	}
	else
	{
		const auto * wtc = scene.get_component<world_transform_component>(m_entityId);
		if (wtc != nullptr)
		{
			log(formatted_string<128>().sprintf("MoveTo world <%f,%f>", m_position.x, m_position.y));
			m_position = transform::get_position(wtc->transform);
		}
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

