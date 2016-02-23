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

const char * Open_Command::get_usage()
{
	return "open <file>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> Open_Command::parse(const std::string & text)
{
	return text.find(' ') == std::string::npos ? std::make_unique<Open_Command>(text) : nullptr;
}

// ============================================================================

Open_Command::Open_Command(std::string _filename) :
	filename(std::move(_filename)),
	entity_ids()
{
}

// ----------------------------------------------------------------------------

static void propagate_add_task(void * mem)
{
	auto entity_ids = static_cast<std::vector<Handle>*>(mem);
	auto & editor = Engine::instance().get_system<Editor_System>();
	for (const Handle entity_id : *entity_ids)
		editor->propagate_add(entity_id);
}

struct open_command_execute_task_data
{
	std::string filename;
	std::vector<Handle> * entity_ids;
};

static void execute_task(void * mem)
{
	auto data = make_unique<open_command_execute_task_data>(mem);
	auto & editor = Engine::instance().get_system<Editor_System>();
	*data->entity_ids = editor->get_scene().load(data->filename.c_str());
	add_task(propagate_add_task, data->entity_ids, 0, task_type::read);
}

void Open_Command::execute()
{
	add_task(execute_task, new open_command_execute_task_data{ std::move(filename), &entity_ids }, Editor_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

bool Open_Command::can_undo() const
{
	return true;
}

// ----------------------------------------------------------------------------

void Open_Command::undo()
{
	const auto & editor = *Engine::instance().get_system<Editor_System>();
	const auto & proxied_scene = editor.get_scene();
	for (const Handle entity_id : entity_ids)
	{
		if (proxied_scene.exists(entity_id))
		{
			proxied_scene.remove_all(entity_id);
			const auto & proxy_scene = Engine::instance().get_current_scene();
			for (const auto & entity : proxy_scene.get_entity_view<Editor_Component>())
				if (entity.get<Editor_Component>().proxied_id == entity_id)
					proxy_scene.remove_all(entity.id);
		}
	}
}

// ============================================================================
