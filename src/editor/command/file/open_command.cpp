// ============================================================================

#include "editor/command/file/open_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/utility/memory.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * open_command::get_usage()
{
	return "open <file>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<command> open_command::parse(const std::string & text)
{
	return text.find(' ') == std::string::npos ? std::make_unique<open_command>(text) : nullptr;
}

// ============================================================================

open_command::open_command(std::string _filename) :
	filename(std::move(_filename)),
	entity_ids()
{
}

// ----------------------------------------------------------------------------

struct execute_task_data
{
	std::string filename;
};

static void execute_task(void * mem)
{
	auto data = make_unique<execute_task_data>(mem);
	auto & editor = engine::instance().get_system<editor_system>();
	auto entity_ids = editor->get_scene().load(data->filename.c_str());
	for (const handle entity_id : entity_ids)
		editor->propagate_add(entity_id);
}

void open_command::execute()
{
	add_task(execute_task, new execute_task_data{ std::move(filename) }, editor_component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

bool open_command::can_undo() const
{
	return true;
}

// ----------------------------------------------------------------------------

void open_command::undo()
{
	const auto & editor = *engine::instance().get_system<editor_system>();
	const auto & scene = editor.get_scene();
	for (const handle entity_id : entity_ids)
	{
		if (scene.exists(entity_id))
		{
			scene.remove_all(entity_id);
			editor.propagated_remove(entity_id);
		}
	}
	entity_ids.clear();
}

// ============================================================================
