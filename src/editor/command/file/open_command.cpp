// ============================================================================

#include "editor/command/file/open_command.h"

#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"

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

void open_command::execute()
{
	engine::instance().add_write_task(engine::instance().get_system<editor_system>()->get_scene(),
		[=](scene & scene)
		{
			entity_ids = scene.load(filename.c_str());
			for (const handle entity_id : entity_ids)
			{
				engine::instance().add_read_task(std::make_unique<delegate_task>(
					[=]()
					{
						engine::instance().get_system<editor_system>()->propagate_add(entity_id);
					}));
			}
		});
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
