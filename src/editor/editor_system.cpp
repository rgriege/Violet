// ============================================================================

#include "editor/editor_system.h"
#include "editor/command/command.h"
#include "editor/component/editor_component.h"
#include "violet/graphics/component/color_component.h"
#include "violet/input/component/mouse_input_component.h"
#include "violet/log/Log.h"
#include "violet/script/script_component.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"

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

void editor_system::init(vlt::deserializer & deserializer)
{
	auto segment = deserializer.enter_segment(get_label_static());
	std::string editScriptFileName = segment->get_string("editScript");

	engine::instance().add_write_task(engine::instance(),
		[=](engine & engine) mutable
		{
			engine.add_system(std::unique_ptr<editor_system>(new editor_system(std::move(editScriptFileName))));
		}, thread::Window);
}

// ----------------------------------------------------------------------------

void editor_system::register_command(const char * const usage, const CommandFactory::producer & producer)
{
	ms_commandFactory.assign(string_utilities::left(usage, ' '), producer);
}

// ============================================================================

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

void editor_system::execute(const std::string & commandString)
{
	const std::string name = string_utilities::left(commandString, ' ');
	auto command = ms_commandFactory.create(name, string_utilities::rightOfFirst(commandString, ' '));
	if (command != nullptr)
		execute(std::move(command));
	else
		log(formatted_string<128>().sprintf("Invalid command '%s'", commandString.c_str()));
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
		engine::instance().add_write_task(engine::instance(),
			[=](engine & engine)
			{
				auto id = entity_id;
				EntitySelectedEvent::emit(engine.event_context, std::move(id));
			});
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
		engine::instance().add_write_task(engine::instance(),
			[=](engine & engine)
			{
				auto id = entity_id;
				EntityDeselectedEvent::emit(engine.event_context, std::move(id));
			});
	}
	return deselected;
}

// ----------------------------------------------------------------------------

void editor_system::propagate_add(const handle entity_id) const
{
	if (m_scene->has_component<world_transform_component>(entity_id) && m_scene->has_component<color_component>(entity_id))
	{
		const auto & engine = engine::instance();
		engine.add_write_task(engine.get_current_scene(),
			[=](scene & scene)
			{
				const auto & editor = *engine::instance().get_system<editor_system>();
				const handle copyId = scene.create_entity();

				scene.create_component<editor_component>(copyId, entity_id);

				scene.create_component<world_transform_component>(copyId, editor.get_scene().get_component<world_transform_component>(entity_id)->transform);

				const auto * cc = editor.get_scene().get_component<color_component>(entity_id);
				scene.create_component<color_component>(copyId, cc->m_mesh->get_poly(), cc->m_shader, cc->m_color);

				const auto * ltc = editor.get_scene().get_component<local_transform_component>(entity_id);
				if (ltc != nullptr)
				{
					handle parentId;
					if (ltc->parent_id != handle::Invalid)
					{
						for (const auto & entity : scene.get_entity_view<editor_component>())
						{
							if (entity.get<editor_component>().edit_id == ltc->parent_id)
							{
								parentId = entity.id;
								break;
							}
						}
					}
					scene.create_component<local_transform_component>(copyId, parentId, ltc->transform);
				}
				editor.addEditBehavior(scene, copyId);
			});
	}
}

// ----------------------------------------------------------------------------

void editor_system::propagated_remove(const handle entity_id) const
{
	const auto & scene = engine::instance().get_current_scene();
	for (const auto & entity : scene.get_entity_view<editor_component>())
		if (entity.get<editor_component>().edit_id == entity_id)
			scene.remove_all(entity.id);
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

void editor_system::addEditBehavior(const scene & scene, const handle entity_id) const
{
	engine::instance().add_read_task(std::make_unique<delegate_task>(
		[&scene, entity_id, this]()
		{
			engine::instance().add_write_task(*scene.get_pool<script_component>(),
				[=](component_pool & pool)
				{
					pool.create<script_component>(entity_id, m_editScriptFileName.c_str());
				});
			poly poly = scene.get_component<color_component>(entity_id)->m_mesh->get_poly();
			engine::instance().add_write_task(*scene.get_pool<mouse_input_component>(),
				[=](component_pool & pool) mutable
				{
					pool.create<mouse_input_component>(entity_id, std::move(poly));
				});
		}), thread::Window);
}

// ============================================================================
