// ============================================================================

#include "editor/command/selection/deselect_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * deselect_command::get_usage()
{
	return "deselect [comma-separated-id-list|all]";
}

// ----------------------------------------------------------------------------

std::unique_ptr<command> deselect_command::parse(const std::string & text)
{
	if (text == "all")
		return std::make_unique<deselect_command>(engine::instance().get_current_scene().get_entity_ids());
	else
	{
		std::vector<std::string> idStrings;
		string_utilities::split(text, ',', idStrings);

		std::vector<handle> entityIds;

		for (const auto & idString : idStrings)
			entityIds.emplace_back(std::atoi(idString.c_str()), ~0);
		return std::make_unique<deselect_command>(std::move(entityIds));
	}
	return nullptr;
}

// ============================================================================

deselect_command::deselect_command(const handle entity_id) :
	m_entityIds()
{
	m_entityIds.emplace_back(entity_id);
}

// ----------------------------------------------------------------------------

deselect_command::deselect_command(const std::vector<handle> & entityIds) :
	m_entityIds(entityIds)
{
}

// ----------------------------------------------------------------------------

deselect_command::deselect_command(std::vector<handle> && entityIds) :
	m_entityIds(std::move(entityIds))
{
}

// ----------------------------------------------------------------------------

struct deselect_command_execute_task_data
{
	std::vector<handle> & entity_ids;
};

static void execute_task(void * mem)
{
	auto data = make_unique<deselect_command_execute_task_data>(mem);
	auto & editor = *engine::instance().get_system<editor_system>();
	data->entity_ids.erase(std::remove_if(data->entity_ids.begin(), data->entity_ids.end(),
		[&](const handle entity_id)
		{
			return !editor.deselect(entity_id);
		}), data->entity_ids.end());
}

void deselect_command::execute()
{
	const auto & engine = engine::instance();
	const auto & editor = *engine.get_system<editor_system>();
	const auto & scene = editor.get_scene();
	for (auto it = m_entityIds.begin(), end = m_entityIds.end(); it != end; )
	{
		auto & entity_id = *it;
		if (entity_id.version == handle::Invalid.version)
			entity_id = handle(entity_id.id, scene.get_entity_version(entity_id.id));

		/*if (!editor.selected(entity_id))
			it = m_entityIds.erase(it);
		else*/
			++it;
	}

	add_task(execute_task, new deselect_command_execute_task_data{ m_entityIds }, editor_component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

bool deselect_command::can_undo() const
{
	return !m_entityIds.empty();
}

// ----------------------------------------------------------------------------

struct deselect_command_undo_task_data
{
	std::vector<handle> entity_ids;
};

static void undo_task(void * mem)
{
	auto data = make_unique<deselect_command_undo_task_data>(mem);
	auto & editor = engine::instance().get_system<editor_system>();
	for (const auto entity_id : data->entity_ids)
		editor->select(entity_id);
}

void deselect_command::undo()
{
	add_task(undo_task, new deselect_command_undo_task_data{ std::move(m_entityIds) }, editor_component::metadata->thread, task_type::write);
}

// ============================================================================
