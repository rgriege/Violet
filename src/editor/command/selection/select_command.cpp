// ============================================================================

#include "editor/command/selection/select_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/log.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * Select_Command::get_usage()
{
	return "select [comma-separated-id-list|all]";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> Select_Command::parse(const std::string & text)
{
	if (text == "all")
		return std::make_unique<Select_Command>(Engine::instance().get_current_scene().get_entity_ids());
	else
	{
		std::vector<std::string> idStrings;
		String_Utilities::split(text, ',', idStrings);

		std::vector<Handle> entityIds;

		for (const auto & idString : idStrings)
			entityIds.emplace_back(std::atoi(idString.c_str()), ~0);
		return std::make_unique<Select_Command>(std::move(entityIds));
	}
	return nullptr;
}

// ============================================================================

Select_Command::Select_Command(const Handle entity_id) :
	m_entityIds()
{
	m_entityIds.emplace_back(entity_id);
}

// ----------------------------------------------------------------------------

Select_Command::Select_Command(const std::vector<Handle> & entityIds) :
	m_entityIds(entityIds)
{
}

// ----------------------------------------------------------------------------

Select_Command::Select_Command(std::vector<Handle> && entityIds) :
	m_entityIds(std::move(entityIds))
{
}

// ----------------------------------------------------------------------------

struct select_command_execute_task_data
{
	std::vector<Handle> & entity_ids;
};

static void execute_task(void * mem)
{
	auto data = make_unique<select_command_execute_task_data>(mem);
	auto & editor = *Engine::instance().get_system<Editor_System>();
	data->entity_ids.erase(std::remove_if(data->entity_ids.begin(), data->entity_ids.end(),
		[&](const Handle entity_id)
		{
			return !editor.select(entity_id);
		}), data->entity_ids.end());
}

void Select_Command::execute()
{
	const auto & Engine = Engine::instance();
	const auto & editor = *Engine.get_system<Editor_System>();
	const auto & Scene = editor.get_scene();
	for (auto it = m_entityIds.begin(), end = m_entityIds.end(); it != end; )
	{
		auto & entity_id = *it;
		if (entity_id.version == Handle::Invalid.version)
			entity_id = Handle(entity_id.id, Scene.get_entity_version(entity_id.id));

		/*if (editor.selected(entity_id))
			it = m_entityIds.erase(it);
		else*/
			++it;
	}		

	add_task(execute_task, new select_command_execute_task_data{ m_entityIds }, Editor_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

bool Select_Command::can_undo() const
{
	return !m_entityIds.empty();
}

// ----------------------------------------------------------------------------

struct select_command_undo_task_data
{
	std::vector<Handle> entity_ids;
};

static void undo_task(void * mem)
{
	auto data = make_unique<select_command_undo_task_data>(mem);
	auto & editor = Engine::instance().get_system<Editor_System>();
	for (const auto entity_id : data->entity_ids)
		editor->deselect(entity_id);
}

void Select_Command::undo()
{
	add_task(undo_task, new select_command_undo_task_data{ std::move(m_entityIds) }, Editor_Component::metadata->thread, task_type::write);
}

// ============================================================================
