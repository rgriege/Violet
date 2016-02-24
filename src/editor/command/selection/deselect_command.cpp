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

		std::vector<Handle> proxy_ids;

		for (const auto & idString : idStrings)
			proxy_ids.emplace_back(std::atoi(idString.c_str()), ~0);
		return std::make_unique<Deselect_Command>(std::move(proxy_ids));
	}
	return nullptr;
}

// ============================================================================

Deselect_Command::Deselect_Command(const Handle entity_id) :
	proxy_ids()
{
	proxy_ids.emplace_back(entity_id);
}

// ----------------------------------------------------------------------------

Deselect_Command::Deselect_Command(const std::vector<Handle> & proxy_ids) :
	proxy_ids(proxy_ids)
{
}

// ----------------------------------------------------------------------------

Deselect_Command::Deselect_Command(std::vector<Handle> && proxy_ids) :
	proxy_ids(std::move(proxy_ids))
{
}

// ----------------------------------------------------------------------------

struct deselect_command_execute_task_data
{
	std::vector<Handle> & proxy_ids;
};

static void execute_task(void * mem)
{
	auto data = make_unique<deselect_command_execute_task_data>(mem);
	auto & editor = *Engine::instance().get_system<Editor_System>();
	data->proxy_ids.erase(std::remove_if(data->proxy_ids.begin(), data->proxy_ids.end(),
		[&](const Handle entity_id)
		{
			return !editor.deselect(entity_id);
		}), data->proxy_ids.end());
}

void Deselect_Command::execute()
{
	const auto & scene = Engine::instance().get_current_scene();
	for (auto & proxy_id : proxy_ids)
		if (proxy_id.version == Handle::Invalid.version)
			proxy_id.version = scene.get_entity_version(proxy_id.id);

	add_task(execute_task, new deselect_command_execute_task_data{ proxy_ids }, Editor_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

bool Deselect_Command::can_undo() const
{
	return !proxy_ids.empty();
}

// ----------------------------------------------------------------------------

struct deselect_command_undo_task_data
{
	std::vector<Handle> proxy_ids;
};

static void undo_task(void * mem)
{
	auto data = make_unique<deselect_command_undo_task_data>(mem);
	auto & editor = Engine::instance().get_system<Editor_System>();
	for (const auto entity_id : data->proxy_ids)
		editor->select(entity_id);
}

void Deselect_Command::undo()
{
	add_task(undo_task, new deselect_command_undo_task_data{ std::move(proxy_ids) }, Editor_Component::metadata->thread, task_type::write);
}

// ============================================================================
