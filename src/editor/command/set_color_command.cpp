// ============================================================================

#include <cstdlib>

#include "editor/command/set_color_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/color_component.h"
#include "violet/log/log.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * set_color_command::get_usage()
{
	return "color <id> <x> <y>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<command> set_color_command::parse(const std::string & text)
{
	std::vector<std::string> arguments;
	string_utilities::split(text, ' ', arguments);
	if (arguments.size() == 2)
	{
		const u32 id = std::atoi(arguments[0].c_str());
		const color c(arguments[1].c_str());
		return std::make_unique<set_color_command>(handle(id, ~0), c);
	}
	return nullptr;
}

// ============================================================================

set_color_command::set_color_command(const handle _entity_id, const color _new_color) :
	entity_id(_entity_id),
	new_color(_new_color)
{
}

// ----------------------------------------------------------------------------

struct set_color_command_store_current_color_task_data
{
	handle entity_id;
	color * color;
};

static void set_color_command_store_current_color_task(void * mem)
{
	auto data = make_unique<set_color_command_store_current_color_task_data>(mem);
	const auto * tc = engine::instance().get_current_scene().get_component<color_component>(data->entity_id);
	*data->color = tc->color;
}

struct set_color_command_set_color_task_data
{
	handle entity_id;
	color color;
};

static void set_color_command_set_color_task(void * mem)
{
	auto data = make_unique<set_color_command_set_color_task_data>(mem);
	auto & proxy_scene = engine::instance().get_current_scene();
	auto * proxy_tc = proxy_scene.get_component<color_component>(data->entity_id);
	if (proxy_tc)
	{
		proxy_tc->color = data->color;

		// todo: async (can't read editor_component here)
		const handle proxied_id = proxy_scene.get_component<editor_component>(data->entity_id)->proxied_id;
		auto * proxied_tc = engine::instance().get_system<editor_system>()->get_scene().get_component<color_component>(proxied_id);
		proxied_tc->color = data->color;
	}
}

void set_color_command::execute()
{
	log(formatted_string<128>().sprintf("set_color::execute %d", entity_id.id));
	const auto & engine = engine::instance();
	const auto & scene = engine.get_current_scene();
	entity_id.version = scene.get_entity_version(entity_id.id);
	
	if (scene.has_component<color_component>(entity_id))
	{
		add_task(set_color_command_store_current_color_task, new set_color_command_store_current_color_task_data{ entity_id, &new_color }, 0, task_type::write);
		add_task(set_color_command_set_color_task, new set_color_command_set_color_task_data{ entity_id, new_color }, 0, task_type::write);
	}
	else
		entity_id.version = handle::Invalid.version;
}

// ----------------------------------------------------------------------------

bool set_color_command::can_undo() const
{
	return entity_id.is_valid();
}

// ----------------------------------------------------------------------------

void set_color_command::undo()
{
	add_task(set_color_command_set_color_task, new set_color_command_set_color_task_data{ entity_id, new_color }, 0, task_type::write);
}

// ============================================================================

