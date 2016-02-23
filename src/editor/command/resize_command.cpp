// ============================================================================

#include <cstdlib>

#include "editor/command/resize_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/color_component.h"
#include "violet/log/log.h"
#include "violet/math/aabb.h"
#include "violet/math/poly.h"
#include "violet/transform/transform.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * Resize_Command::get_usage()
{
	return "resize <id> <x> <y>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> Resize_Command::parse(const std::string & text)
{
	std::vector<std::string> arguments;
	String_Utilities::split(text, ' ', arguments);
	if (arguments.size() == 3)
	{
		const u32 id = std::atoi(arguments[0].c_str());
		const r32 w = static_cast<r32>(std::atof(arguments[1].c_str()));
		const r32 h = static_cast<r32>(std::atof(arguments[2].c_str()));
		return std::make_unique<Resize_Command>(Handle(id, ~0), v2(w, h));
	}
	return nullptr;
}

// ============================================================================

Resize_Command::Resize_Command(const Handle _entity_id, const v2 & _dimensions) :
	entity_id(_entity_id),
	dimensions(_dimensions)
{
}

// ----------------------------------------------------------------------------

struct resize_command_store_current_dimensions_task_data
{
	Handle entity_id;
	v2 * dimensions;
};

static void resize_command_store_current_dimensions_task(void * mem)
{
	auto data = make_unique<resize_command_store_current_dimensions_task_data>(mem);
	const auto * cc = Engine::instance().get_current_scene().get_component<Color_Component>(data->entity_id);
	const aabb bounds = cc->mesh->get_poly().get_bounding_box();
	*data->dimensions = v2(bounds.bottom_right.x - bounds.top_left.x, bounds.top_left.y - bounds.bottom_right.y);
}

struct resize_command_set_dimensions_task_data
{
	Handle entity_id;
	v2 dimensions;
};

static void resize_command_set_dimensions_task(void * mem)
{
	auto data = make_unique<resize_command_set_dimensions_task_data>(mem);
	auto & proxy_scene = Engine::instance().get_current_scene();
	auto * proxy_cc = proxy_scene.get_component<Color_Component>(data->entity_id);
	if (proxy_cc)
	{
		aabb box(proxy_cc->mesh->get_poly().get_center(), v2::Zero);
		box.extend(-data->dimensions / 2);
		box.extend(data->dimensions / 2);
		const Poly p(box);
		set_mesh_vertices(*proxy_cc->mesh, p.vertices);

		// todo: async (can't ready Editor_Component here)
		const Handle proxied_id = proxy_scene.get_component<Editor_Component>(data->entity_id)->proxied_id;
		auto * proxied_cc = Engine::instance().get_system<Editor_System>()->get_scene().get_component<Color_Component>(proxied_id);
		set_mesh_vertices(*proxied_cc->mesh, p.vertices);
	}
}

void Resize_Command::execute()
{
	log(Formatted_String<128>().sprintf("resize::execute %d", entity_id.id));
	const auto & Engine = Engine::instance();
	const auto & Scene = Engine.get_current_scene();
	entity_id.version = Scene.get_entity_version(entity_id.id);
	
	if (Scene.has_component<Color_Component>(entity_id))
	{
		add_task(resize_command_store_current_dimensions_task, new resize_command_store_current_dimensions_task_data{ entity_id, &dimensions }, 0, task_type::write);
		add_task(resize_command_set_dimensions_task, new resize_command_set_dimensions_task_data{ entity_id, dimensions }, 0, task_type::write);
	}
	else
		entity_id.version = Handle::Invalid.version;
}

// ----------------------------------------------------------------------------

bool Resize_Command::can_undo() const
{
	return entity_id.is_valid();
}

// ----------------------------------------------------------------------------

void Resize_Command::undo()
{
	add_task(resize_command_set_dimensions_task, new resize_command_set_dimensions_task_data{ entity_id, dimensions }, 0, task_type::write);
}

// ============================================================================

