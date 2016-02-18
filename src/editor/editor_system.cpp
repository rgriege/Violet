// ============================================================================

#include "editor/editor_system.h"
#include "editor/command/command.h"
#include "editor/component/editor_component.h"
#include "violet/graphics/component/color_component.h"
#include "violet/graphics/component/text_component.h"
#include "violet/input/component/mouse_input_component.h"
#include "violet/log/Log.h"
#include "violet/script/script_component.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"

#include <deque>

using namespace edt;
using namespace vlt;

// ============================================================================

namespace EditorNamespace
{
	editor_system::CommandFactory ms_commandFactory;
}

using namespace EditorNamespace;

// ============================================================================

const char * editor_system::get_label_static()
{
	return "edtr";
}

// ----------------------------------------------------------------------------

void editor_system::install(vlt::system_factory & factory)
{
	factory.assign(get_label_static(), &editor_system::init);
}

// ----------------------------------------------------------------------------

struct init_task_data
{
	std::string edit_scrit_filename;
};

static void init_task(void * mem)
{
	auto data = make_unique<init_task_data>(mem);
	engine::instance().add_system(std::unique_ptr<editor_system>(new editor_system(std::move(data->edit_scrit_filename))));
}

void editor_system::init(vlt::deserializer & deserializer)
{
	auto segment = deserializer.enter_segment(get_label_static());
	auto data = new init_task_data;
	data->edit_scrit_filename = segment->get_string("editScript");
	add_task(init_task, data, editor_component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

void editor_system::register_command(const char * const usage, const CommandFactory::producer & producer)
{
	ms_commandFactory.assign(string_utilities::left(usage, ' '), producer);
}

// ============================================================================

editor_system::editor_system(std::string editScriptFileName) :
	system(get_label_static()),
	m_scene(std::make_unique<scene>()),
	m_editScriptFileName(std::move(editScriptFileName)),
	m_commandHistory(),
	m_selectedEntities()
{
}

// ----------------------------------------------------------------------------

/*void editor_system::update(const r32 dt)
{
	const auto & engine = engine::instance();
	const auto & scene = engine.get_current_scene();
	std::vector<handle> edittedEntityIds;

	{
		const auto & edittedEntities = scene.get_entity_view<editor_component>();
		entityIds.reserve(edittedEntities.size());
		for (const auto & entity : edittedEntities)
		{
			const handle entity_id = std::get<0>(entity).getEdittedId();
			edittedEntityIds.emplace_back(entity_id);
			if (!m_scene->hasEntity(entity_id))
			{
				engine.add_write_task(scene,
					[=](scene & scene)
					{
						scene.remove_all(entity_id);
					});
			}
		}
	}
		
	for (const auto & entity : scene.get_entity_view<local_transform_component>())
		if (!m_scene->hasEntity(std::get<0>(entity).get_entity_id()))
			engine.add_write_task
}*/

// ----------------------------------------------------------------------------

scene & editor_system::get_scene()
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

const scene & editor_system::get_scene() const
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

vlt::handle editor_system::get_proxy_id(const handle proxied_id) const
{
	vlt::handle proxy_id;
	for (const auto & entity : engine::instance().get_current_scene().get_entity_view<edt::editor_component>())
		if (entity.get<edt::editor_component>().proxied_id == proxied_id)
			proxy_id = entity.id;
	return proxy_id;
}

// ----------------------------------------------------------------------------

b8 editor_system::execute(const std::string & commandString)
{
	const std::string name = string_utilities::left(commandString, ' ');
	if (ms_commandFactory.has(name))
	{
		auto command = ms_commandFactory.create(name, string_utilities::rightOfFirst(commandString, ' '));
		if (command != nullptr)
		{
			execute(std::move(command));
			return true;
		}
	}

	log(formatted_string<128>().sprintf("Invalid command '%s'", commandString.c_str()));
	return false;
}

// ----------------------------------------------------------------------------

void editor_system::execute(std::unique_ptr<command> && command)
{
	command->execute();
	if (command->can_undo())
	{
		m_commandHistory.emplace_back(std::move(command));
		if (m_commandHistory.size() >= 100)
			m_commandHistory.pop_front();
	}
}

// ----------------------------------------------------------------------------

void editor_system::undo()
{
	if (!m_commandHistory.empty())
	{
		m_commandHistory.back()->undo();
		m_commandHistory.pop_back();
	}
}

// ----------------------------------------------------------------------------

bool editor_system::select(const handle entity_id)
{
	const bool selected = m_selectedEntities.emplace(entity_id).second;
	if (selected)
	{
		auto id = entity_id;
		EntitySelectedEvent::emit(engine::instance(), std::move(id));
	}
	return selected;
}

// ----------------------------------------------------------------------------

bool editor_system::selected(const handle entity_id) const
{
	return m_selectedEntities.find(entity_id) != m_selectedEntities.end();
}

// ----------------------------------------------------------------------------

const std::set<handle> & editor_system::get_selected_entities() const
{
	return m_selectedEntities;
}

// ----------------------------------------------------------------------------

bool editor_system::deselect(const handle entity_id)
{
	const bool deselected = m_selectedEntities.erase(entity_id) != 0;
	if (deselected)
	{
		auto id = entity_id;
		EntityDeselectedEvent::emit(engine::instance().event_context, std::move(id));
	}
	return deselected;
}

// ----------------------------------------------------------------------------

struct editor_system_propagate_add_task_data
{
	handle entity_id;
	std::string edit_script_filename;
};

static void propagate_add_task(void * mem)
{
	auto data = make_unique<editor_system_propagate_add_task_data>(mem);
	auto & scene = engine::instance().get_current_scene();
	const auto & editor = *engine::instance().get_system<editor_system>();
	// this thread
	const handle proxy_id = scene.create_entity();

	// new thread
	scene.create_component<editor_component>(proxy_id, data->entity_id);

	// new thread
	scene.create_component<world_transform_component>(proxy_id, editor.get_scene().get_component<world_transform_component>(data->entity_id)->transform);

	// new thread
	poly poly(aabb{});
	const auto * cc = editor.get_scene().get_component<color_component>(data->entity_id);
	if (cc)
	{
		poly = cc->m_mesh->get_poly();
		scene.create_component<color_component>(proxy_id, poly, cc->m_shader, cc->color);
	}

	// new thread
	const auto * tc = editor.get_scene().get_component<text_component>(data->entity_id);
	if (tc)
	{
		poly = tc->m_mesh->get_poly();
		scene.create_component<text_component>(proxy_id, *tc);
	}

	const auto * ltc = editor.get_scene().get_component<local_transform_component>(data->entity_id);
	if (ltc != nullptr)
	{
		handle parentId;
		if (ltc->parent_id != handle::Invalid)
		{
			for (const auto & entity : scene.get_entity_view<editor_component>())
			{
				if (entity.get<editor_component>().proxied_id == ltc->parent_id)
				{
					parentId = entity.id;
					break;
				}
			}
		}
		// new thread
		scene.create_component<local_transform_component>(proxy_id, parentId, ltc->transform);
	}
	// new thread
	scene.create_component<script_component>(proxy_id, data->edit_script_filename.c_str());

	scene.create_component<mouse_input_component>(proxy_id, std::move(poly));
}

void editor_system::propagate_add(const handle entity_id) const
{
	if (   m_scene->has_component<world_transform_component>(entity_id)
		&& (   m_scene->has_component<color_component>(entity_id)
			|| m_scene->has_component<text_component>(entity_id)))
		add_task(propagate_add_task, new editor_system_propagate_add_task_data{ entity_id, m_editScriptFileName }, editor_component::metadata->thread, task_type::write);
}

// ============================================================================
