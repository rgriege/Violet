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
		std::vector<std::string> id_strings;
		String_Utilities::split(text, ',', id_strings);

		std::vector<Handle> proxy_ids;

		for (const auto & id_string : id_strings)
			proxy_ids.emplace_back(std::atoi(id_string.c_str()), ~0);
		return std::make_unique<Select_Command>(std::move(proxy_ids));
	}
	return nullptr;
}

// ============================================================================

Select_Command::Select_Command(const Handle proxy_id) :
	proxy_ids()
{
	proxy_ids.emplace_back(proxy_id);
}

// ----------------------------------------------------------------------------

Select_Command::Select_Command(const std::vector<Handle> & proxy_ids) :
	proxy_ids(proxy_ids)
{
}

// ----------------------------------------------------------------------------

Select_Command::Select_Command(std::vector<Handle> && proxy_ids) :
	proxy_ids(std::move(proxy_ids))
{
}

// ----------------------------------------------------------------------------

struct select_command_execute_task_data
{
	std::vector<Handle> & proxy_ids;
};

static void execute_task(void * mem)
{
	auto data = make_unique<select_command_execute_task_data>(mem);
	auto & editor = *Engine::instance().get_system<Editor_System>();
	data->proxy_ids.erase(std::remove_if(data->proxy_ids.begin(), data->proxy_ids.end(),
		[&](const Handle proxy_id)
		{
			return !editor.select(proxy_id);
		}), data->proxy_ids.end());
}

void Select_Command::execute()
{
	const auto & scene = Engine::instance().get_current_scene();
	for (auto & proxy_id : proxy_ids)
		if (proxy_id.version == Handle::Invalid.version)
			proxy_id.version = scene.get_entity_version(proxy_id.id);

	add_task(execute_task, new select_command_execute_task_data{ proxy_ids }, Editor_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

bool Select_Command::can_undo() const
{
	return !proxy_ids.empty();
}

// ----------------------------------------------------------------------------

struct select_command_undo_task_data
{
	std::vector<Handle> proxy_ids;
};

static void undo_task(void * mem)
{
	auto data = make_unique<select_command_undo_task_data>(mem);
	auto & editor = Engine::instance().get_system<Editor_System>();
	for (const auto proxy_id : data->proxy_ids)
		editor->deselect(proxy_id);
}

void Select_Command::undo()
{
	add_task(undo_task, new select_command_undo_task_data{ std::move(proxy_ids) }, Editor_Component::metadata->thread, task_type::write);
}

// ============================================================================
