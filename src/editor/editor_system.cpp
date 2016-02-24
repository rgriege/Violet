// ============================================================================

#include <deque>

#include "editor/command/command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/graphics/component/color_component.h"
#include "violet/graphics/component/text_component.h"
#include "violet/input/component/mouse_input_component.h"
#include "violet/log/log.h"
#include "violet/script/script_component.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"

using namespace edt;
using namespace vlt;

// ============================================================================

namespace EditorNamespace
{
	Editor_System::CommandFactory ms_commandFactory;
}

using namespace EditorNamespace;

// ============================================================================

const char * Editor_System::get_label_static()
{
	return "edtr";
}

// ----------------------------------------------------------------------------

void Editor_System::install(vlt::System_Factory & factory)
{
	factory.assign(get_label_static(), &Editor_System::init);
}

// ----------------------------------------------------------------------------

struct init_task_data
{
	std::string edit_script_filename;
};

static void init_task(void * mem)
{
	auto data = make_unique<init_task_data>(mem);
	Engine::instance().add_system(std::unique_ptr<Editor_System>(new Editor_System(std::move(data->edit_script_filename))));
}

void Editor_System::init(vlt::Deserializer & deserializer)
{
	auto segment = deserializer.enter_segment(get_label_static());
	auto data = new init_task_data;
	data->edit_script_filename = segment->get_string("editScript");
	add_task(init_task, data, Editor_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

void Editor_System::register_command(const char * const usage, const CommandFactory::Producer & producer)
{
	ms_commandFactory.assign(String_Utilities::left(usage, ' '), producer);
}

// ============================================================================

Editor_System::Editor_System(std::string editScriptFileName) :
	System(get_label_static()),
	m_scene(std::make_unique<Scene>()),
	m_editScriptFileName(std::move(editScriptFileName)),
	m_commandHistory(),
	selected_proxies()
{
}

// ----------------------------------------------------------------------------

/*void Editor_System::update(const r32 dt)
{
	const auto & Engine = Engine::instance();
	const auto & Scene = Engine.get_current_scene();
	std::Vector<Handle> edittedEntityIds;

	{
		const auto & edittedEntities = Scene.get_entity_view<Editor_Component>();
		entityIds.reserve(edittedEntities.size());
		for (const auto & entity : edittedEntities)
		{
			const Handle entity_id = std::get<0>(entity).getEdittedId();
			edittedEntityIds.emplace_back(entity_id);
			if (!m_scene->hasEntity(entity_id))
			{
				Engine.add_write_task(Scene,
					[=](Scene & Scene)
					{
						Scene.remove_all(entity_id);
					});
			}
		}
	}
		
	for (const auto & entity : Scene.get_entity_view<Local_Transform_Component>())
		if (!m_scene->hasEntity(std::get<0>(entity).get_entity_id()))
			Engine.add_write_task
}*/

// ----------------------------------------------------------------------------

Scene & Editor_System::get_scene()
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

const Scene & Editor_System::get_scene() const
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

vlt::Handle Editor_System::get_proxy_id(const Handle proxied_id) const
{
	vlt::Handle proxy_id;
	for (const auto & entity : Engine::instance().get_current_scene().get_entity_view<edt::Editor_Component>())
		if (entity.get<edt::Editor_Component>().proxied_id == proxied_id)
			proxy_id = entity.id;
	return proxy_id;
}

// ----------------------------------------------------------------------------

b8 Editor_System::execute(const std::string & commandString)
{
	const std::string name = String_Utilities::left(commandString, ' ');
	if (ms_commandFactory.has(name))
	{
		auto command = ms_commandFactory.create(name, String_Utilities::rightOfFirst(commandString, ' '));
		if (command != nullptr)
		{
			execute(std::move(command));
			return true;
		}
	}

	log(Formatted_String<128>().sprintf("Invalid Command '%s'", commandString.c_str()));
	return false;
}

// ----------------------------------------------------------------------------

void Editor_System::execute(std::unique_ptr<Command> && command)
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

void Editor_System::undo()
{
	if (!m_commandHistory.empty())
	{
		m_commandHistory.back()->undo();
		m_commandHistory.pop_back();
	}
}

// ----------------------------------------------------------------------------

bool Editor_System::select(const Handle proxy_id)
{
	const bool selected = selected_proxies.emplace(proxy_id).second;
	if (selected)
	{
		auto id = proxy_id;
		EntitySelectedEvent::emit(Engine::instance(), std::move(id));
	}
	return selected;
}

// ----------------------------------------------------------------------------

bool Editor_System::selected(const Handle proxy_id) const
{
	return selected_proxies.find(proxy_id) != selected_proxies.end();
}

// ----------------------------------------------------------------------------

const std::set<Handle> & Editor_System::get_selected_proxies() const
{
	return selected_proxies;
}

// ----------------------------------------------------------------------------

bool Editor_System::deselect(const Handle proxy_id)
{
	const bool deselected = selected_proxies.erase(proxy_id) != 0;
	if (deselected)
	{
		auto id = proxy_id;
		EntityDeselectedEvent::emit(Engine::instance().event_context, std::move(id));
	}
	return deselected;
}

// ----------------------------------------------------------------------------

struct editor_system_propagate_add_task_data
{
	Handle entity_id;
	std::string edit_script_filename;
};

static void propagate_add_task(void * mem)
{
	auto data = make_unique<editor_system_propagate_add_task_data>(mem);
	auto & scene = Engine::instance().get_current_scene();
	const auto & editor = *Engine::instance().get_system<Editor_System>();
	// this Thread
	const Handle proxy_id = scene.create_entity();

	// new Thread
	scene.create_component<Editor_Component>(proxy_id, data->entity_id);

	// new Thread
	scene.create_component<World_Transform_Component>(proxy_id, editor.get_scene().get_component<World_Transform_Component>(data->entity_id)->transform);

	// new Thread
	Poly poly(aabb{});
	const auto * cc = editor.get_scene().get_component<Color_Component>(data->entity_id);
	if (cc)
	{
		poly = cc->mesh->get_poly();
		scene.create_component<Color_Component>(proxy_id, poly, cc->shader, cc->color);
	}

	// new Thread
	const auto * tc = editor.get_scene().get_component<Text_Component>(data->entity_id);
	if (tc)
	{
		poly = tc->mesh->get_poly();
		scene.create_component<Text_Component>(proxy_id, *tc);
	}

	const auto * ltc = editor.get_scene().get_component<Local_Transform_Component>(data->entity_id);
	if (ltc != nullptr)
	{
		Handle parentId;
		if (ltc->parent_id != Handle::Invalid)
		{
			for (const auto & entity : scene.get_entity_view<Editor_Component>())
			{
				if (entity.get<Editor_Component>().proxied_id == ltc->parent_id)
				{
					parentId = entity.id;
					break;
				}
			}
		}
		// new Thread
		scene.create_component<Local_Transform_Component>(proxy_id, parentId, ltc->transform);
	}
	// new Thread
	scene.create_component<Script_Component>(proxy_id, data->edit_script_filename.c_str());

	scene.create_component<Mouse_Input_Component>(proxy_id, std::move(poly));
}

void Editor_System::propagate_add(const Handle entity_id) const
{
	if (   m_scene->has_component<World_Transform_Component>(entity_id)
		&& (   m_scene->has_component<Color_Component>(entity_id)
			|| m_scene->has_component<Text_Component>(entity_id)))
		add_task(propagate_add_task, new editor_system_propagate_add_task_data{ entity_id, m_editScriptFileName }, Editor_Component::metadata->thread, task_type::write);
}

// ============================================================================
