// ============================================================================

#include <cstdlib>

#include "editor/command/set_text_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/text_component.h"
#include "violet/log/log.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * Set_Text_Command::get_usage()
{
	return "text <id> <x> <y>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> Set_Text_Command::parse(const std::string & text)
{
	std::vector<std::string> arguments;
	String_Utilities::split(text, ' ', arguments);
	if (arguments.size() == 2)
	{
		const u32 id = std::atoi(arguments[0].c_str());
		return std::make_unique<Set_Text_Command>(Handle(id, ~0), arguments[1]);
	}
	return nullptr;
}

// ============================================================================

Set_Text_Command::Set_Text_Command(const Handle _entity_id, const std::string & _text) :
	entity_id(_entity_id),
	text(_text)
{
}

// ----------------------------------------------------------------------------

struct set_text_command_store_current_text_task_data
{
	Handle entity_id;
	std::string * text;
};

static void set_text_command_store_current_text_task(void * mem)
{
	auto data = make_unique<set_text_command_store_current_text_task_data>(mem);
	const auto * tc = Engine::instance().get_current_scene().get_component<Text_Component>(data->entity_id);
	*data->text = tc->text;
}

struct set_text_command_set_text_task_data
{
	Handle entity_id;
	std::string text;
};

static void set_text_command_set_text_task(void * mem)
{
	auto data = make_unique<set_text_command_set_text_task_data>(mem);
	auto & proxy_scene = Engine::instance().get_current_scene();
	auto * proxy_tc = proxy_scene.get_component<Text_Component>(data->entity_id);
	if (proxy_tc)
	{
		proxy_tc->text = data->text;

		// todo: async (can't read Editor_Component here)
		const Handle proxied_id = proxy_scene.get_component<Editor_Component>(data->entity_id)->proxied_id;
		auto * proxied_tc = Engine::instance().get_system<Editor_System>()->get_scene().get_component<Text_Component>(proxied_id);
		proxied_tc->text = data->text;
	}
}

void Set_Text_Command::execute()
{
	log(Formatted_String<128>().sprintf("set_text::execute %d", entity_id.id));
	const auto & engine = Engine::instance();
	const auto & scene = engine.get_current_scene();
	entity_id.version = scene.get_entity_version(entity_id.id);
	
	if (scene.has_component<Text_Component>(entity_id))
	{
		add_task(set_text_command_store_current_text_task, new set_text_command_store_current_text_task_data{ entity_id, &text }, 0, task_type::write);
		add_task(set_text_command_set_text_task, new set_text_command_set_text_task_data{ entity_id, text }, 0, task_type::write);
	}
	else
		entity_id.version = Handle::Invalid.version;
}

// ----------------------------------------------------------------------------

bool Set_Text_Command::can_undo() const
{
	return entity_id.is_valid();
}

// ----------------------------------------------------------------------------

void Set_Text_Command::undo()
{
	add_task(set_text_command_set_text_task, new set_text_command_set_text_task_data{ entity_id, text }, 0, task_type::write);
}

// ============================================================================

