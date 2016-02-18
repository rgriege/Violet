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

static void propagate_add_task(void * mem)
{
	auto entity_ids = static_cast<std::vector<handle>*>(mem);
	auto & editor = engine::instance().get_system<editor_system>();
	for (const handle entity_id : *entity_ids)
		editor->propagate_add(entity_id);
}

struct open_command_execute_task_data
{
	std::string filename;
	std::vector<handle> * entity_ids;
};

static void execute_task(void * mem)
{
	auto data = make_unique<open_command_execute_task_data>(mem);
	auto & editor = engine::instance().get_system<editor_system>();
	*data->entity_ids = editor->get_scene().load(data->filename.c_str());
	add_task(propagate_add_task, data->entity_ids, 0, task_type::read);
}

void open_command::execute()
{
	add_task(execute_task, new open_command_execute_task_data{ std::move(filename), &entity_ids }, editor_component::metadata->thread, task_type::write);
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
	const auto & proxied_scene = editor.get_scene();
	for (const handle entity_id : entity_ids)
	{
		if (proxied_scene.exists(entity_id))
		{
			proxied_scene.remove_all(entity_id);
			const auto & proxy_scene = engine::instance().get_current_scene();
			for (const auto & entity : proxy_scene.get_entity_view<editor_component>())
				if (entity.get<editor_component>().proxied_id == entity_id)
					proxy_scene.remove_all(entity.id);
		}
	}
}

// ============================================================================
