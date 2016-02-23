// ============================================================================

#include "editor/command/selection/deselect_command.h"
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

const char * Deselect_Command::get_usage()
{
	return "deselect [comma-separated-id-list|all]";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> Deselect_Command::parse(const std::string & text)
{
	if (text == "all")
		return std::make_unique<Deselect_Command>(Engine::instance().get_current_scene().get_entity_ids());
	else
	{
		std::vector<std::string> idStrings;
		String_Utilities::split(text, ',', idStrings);

		std::vector<Handle> entityIds;

		for (const auto & idString : idStrings)
			entityIds.emplace_back(std::atoi(idString.c_str()), ~0);
		return std::make_unique<Deselect_Command>(std::move(entityIds));
	}
	return nullptr;
}

// ============================================================================

Deselect_Command::Deselect_Command(const Handle entity_id) :
	m_entityIds()
{
	m_entityIds.emplace_back(entity_id);
}

// ----------------------------------------------------------------------------

Deselect_Command::Deselect_Command(const std::vector<Handle> & entityIds) :
	m_entityIds(entityIds)
{
}

// ----------------------------------------------------------------------------

Deselect_Command::Deselect_Command(std::vector<Handle> && entityIds) :
	m_entityIds(std::move(entityIds))
{
}

// ----------------------------------------------------------------------------

struct deselect_command_execute_task_data
{
	std::vector<Handle> & entity_ids;
};

static void execute_task(void * mem)
{
	auto data = make_unique<deselect_command_execute_task_data>(mem);
	auto & editor = *Engine::instance().get_system<Editor_System>();
	data->entity_ids.erase(std::remove_if(data->entity_ids.begin(), data->entity_ids.end(),
		[&](const Handle entity_id)
		{
			return !editor.deselect(entity_id);
		}), data->entity_ids.end());
}

void Deselect_Command::execute()
{
	const auto & Engine = Engine::instance();
	const auto & editor = *Engine.get_system<Editor_System>();
	const auto & Scene = editor.get_scene();
	for (auto it = m_entityIds.begin(), end = m_entityIds.end(); it != end; )
	{
		auto & entity_id = *it;
		if (entity_id.version == Handle::Invalid.version)
			entity_id = Handle(entity_id.id, Scene.get_entity_version(entity_id.id));

		/*if (!editor.selected(entity_id))
			it = m_entityIds.erase(it);
		else*/
			++it;
	}

	add_task(execute_task, new deselect_command_execute_task_data{ m_entityIds }, Editor_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

bool Deselect_Command::can_undo() const
{
	return !m_entityIds.empty();
}

// ----------------------------------------------------------------------------

struct deselect_command_undo_task_data
{
	std::vector<Handle> entity_ids;
};

static void undo_task(void * mem)
{
	auto data = make_unique<deselect_command_undo_task_data>(mem);
	auto & editor = Engine::instance().get_system<Editor_System>();
	for (const auto entity_id : data->entity_ids)
		editor->select(entity_id);
}

void Deselect_Command::undo()
{
	add_task(undo_task, new deselect_command_undo_task_data{ std::move(m_entityIds) }, Editor_Component::metadata->thread, task_type::write);
}

// ============================================================================
