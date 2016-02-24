#include "editor/command/file/open_command.h"
#include "editor/command/file/save_all_command.h"
#include "editor/component/editor_component.h"
#include "editor/cpp_script_exports.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/text_component.h"
#include "violet/input/system/input_system.h"
#include "violet/log/log.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"

#include "dialog.h"

using namespace edt;
using namespace vlt;

struct assign_label_task_data
{
	Handle label_id;
	Handle proxy_id;
};

static void assign_label_task(void * mem)
{
	auto data = make_unique<assign_label_task_data>(mem);
	auto * tc = Engine::instance().get_current_scene().get_component<Text_Component>(data->label_id);
	if (tc)
		tc->text = Formatted_String<16>().sprintf("%d", data->proxy_id.id);

	auto * ltc = Engine::instance().get_current_scene().get_component<Local_Transform_Component>(data->label_id);
	if (ltc)
		ltc->parent_id = data->proxy_id;
}

static void show_labels_task(void * mem)
{
	auto label_entities = static_cast<std::vector<Handle>*>(mem);
	auto & scene = Engine::instance().get_current_scene();
	const auto current_entity_ids = scene.get_entity_ids();
	for (const Handle entity_id : current_entity_ids)
	{
		if (scene.has_component<Editor_Component>(entity_id))
		{
			label_entities->emplace_back(scene.load("editor_label.json").front());
			add_task(assign_label_task, new assign_label_task_data{ label_entities->back(), entity_id }, 0, task_type::write);
		}
	}
}

static void hide_labels_task(void * mem)
{
	auto label_entities = static_cast<std::vector<Handle>*>(mem);
	auto & scene = Engine::instance().get_current_scene();
	for (const Handle entity_id : *label_entities)
		scene.remove_all(entity_id);
	label_entities->clear();
}

static void load_dialog_task(void * mem)
{
	auto dialog_entities = static_cast<std::vector<Handle>*>(mem);
	*dialog_entities = Engine::instance().get_current_scene().load("dialog.json");
}

static void remove_dialog_task(void * mem)
{
	auto dialog_entities = make_unique<std::vector<Handle>>(mem);
	auto & scene = Engine::instance().get_current_scene();
	for (const Handle entity_id : *dialog_entities)
		scene.remove_all(entity_id);
}

struct Instance final : public Cpp_Script::Instance
{
	Instance(Cpp_Script & script) :
		Cpp_Script::Instance(script),
		dialog_entities(),
		label_entities()
	{
		KeyUpMethod::assign(script, KeyUpMethod::Handler::bind<Instance, &Instance::on_key_up>(this));
		DialogClosedEvent::subscribe(Engine::instance(), DialogClosedEvent::Subscriber::bind<Instance, &Instance::onDialogClosed>(this));
	}

	virtual ~Instance() override
	{
		KeyUpMethod::remove(script);
		DialogClosedEvent::unsubscribe(Engine::instance(), DialogClosedEvent::Subscriber::bind<Instance, &Instance::onDialogClosed>(this));
	}

	void on_key_up(const Handle entityId, const Window_System::Key_Event & event)
	{
		// todo: async
		switch (event.code)
		{
		case 'd':
			if ((event.modifiers & Key::Modifier::M_CTRL) == 0)
			{
				Engine::instance().get_system<Editor_System>()->execute(create_clear_command());
			}
			break;

		case 't':
			if (label_entities.empty())
				add_task(show_labels_task, &label_entities, 0, task_type::write);
			else
				add_task(hide_labels_task, &label_entities, 0, task_type::write);
			break;

		case 'z':
			if ((event.modifiers & Key::Modifier::M_CTRL) != 0)
			{
				Engine::instance().get_system<Editor_System>()->undo();
			}
			break;

		case ';':
			if (event.modifiers & Key::Modifier::M_SHIFT)
			{
				add_task(load_dialog_task, &dialog_entities, 0, task_type::write);
			}
			break;
		}
	}

	void onDialogClosed(const std::string & input)
	{
		// todo: async (execute, remove_all)
		if (!input.empty())
			Engine::instance().get_system<Editor_System>()->execute(input);

		add_task(remove_dialog_task, new std::vector<Handle>(std::move(dialog_entities)), 0, task_type::write);
		dialog_entities.clear();
	}

private:

	std::vector<Handle> dialog_entities;
	std::vector<Handle> label_entities;
};

VIOLET_SCRIPT_EXPORT void init(Cpp_Script & script, std::unique_ptr<Cpp_Script::Instance> & i)
{
	i = std::make_unique<Instance>(script);
}

