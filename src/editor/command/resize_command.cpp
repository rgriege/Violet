// ============================================================================

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

#include <cstdlib>

using namespace edt;
using namespace vlt;

// ============================================================================

const char * resize_command::get_usage()
{
	return "resize <id> <x> <y>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<command> resize_command::parse(const std::string & text)
{
	std::vector<std::string> arguments;
	string_utilities::split(text, ' ', arguments);
	if (arguments.size() == 3)
	{
		const u32 id = std::atoi(arguments[0].c_str());
		const r32 w = static_cast<r32>(std::atof(arguments[1].c_str()));
		const r32 h = static_cast<r32>(std::atof(arguments[2].c_str()));
		return std::make_unique<resize_command>(handle(id, ~0), v2(w, h));
	}
	return nullptr;
}

// ============================================================================

resize_command::resize_command(const handle _entity_id, const v2 & _dimensions) :
	entity_id(_entity_id),
	dimensions(_dimensions)
{
}

// ----------------------------------------------------------------------------

struct resize_command_store_current_dimensions_task_data
{
	handle entity_id;
	v2 * dimensions;
};

static void resize_command_store_current_dimensions_task(void * mem)
{
	auto data = make_unique<resize_command_store_current_dimensions_task_data>(mem);
	const auto * cc = engine::instance().get_current_scene().get_component<color_component>(data->entity_id);
	const aabb bounds = cc->m_mesh->get_poly().get_bounding_box();
	*data->dimensions = v2(bounds.bottom_right.x - bounds.top_left.x, bounds.top_left.y - bounds.bottom_right.y);
}

struct resize_command_set_dimensions_task_data
{
	handle entity_id;
	v2 dimensions;
};

static void resize_command_set_dimensions_task(void * mem)
{
	auto data = make_unique<resize_command_set_dimensions_task_data>(mem);
	auto & proxy_scene = engine::instance().get_current_scene();
	auto * proxy_cc = proxy_scene.get_component<color_component>(data->entity_id);
	if (proxy_cc)
	{
		aabb box(proxy_cc->m_mesh->get_poly().get_center(), v2::Zero);
		box.extend(-data->dimensions / 2);
		box.extend(data->dimensions / 2);
		const poly p(box);
		set_mesh_vertices(*proxy_cc->m_mesh, p.vertices);

		// todo: async (can't ready editor_component here)
		const handle proxied_id = proxy_scene.get_component<editor_component>(data->entity_id)->proxied_id;
		auto * proxied_cc = engine::instance().get_system<editor_system>()->get_scene().get_component<color_component>(proxied_id);
		set_mesh_vertices(*proxied_cc->m_mesh, p.vertices);
	}
}

void resize_command::execute()
{
	log(formatted_string<128>().sprintf("resize::execute %d", entity_id.id));
	const auto & engine = engine::instance();
	const auto & scene = engine.get_current_scene();
	entity_id.version = scene.get_entity_version(entity_id.id);
	
	if (scene.has_component<color_component>(entity_id))
	{
		add_task(resize_command_store_current_dimensions_task, new resize_command_store_current_dimensions_task_data{ entity_id, &dimensions }, 0, task_type::write);
		add_task(resize_command_set_dimensions_task, new resize_command_set_dimensions_task_data{ entity_id, dimensions }, 0, task_type::write);
	}
	else
		entity_id.version = handle::Invalid.version;
}

// ----------------------------------------------------------------------------

bool resize_command::can_undo() const
{
	return entity_id.is_valid();
}

// ----------------------------------------------------------------------------

void resize_command::undo()
{
	add_task(resize_command_set_dimensions_task, new resize_command_set_dimensions_task_data{ entity_id, dimensions }, 0, task_type::write);
}

// ============================================================================

