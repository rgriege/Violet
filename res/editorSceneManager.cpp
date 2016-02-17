#include "editor/editor_system.h"
#include "editor/cpp_script_exports.h"
#include "editor/command/file/open_command.h"
#include "editor/command/file/save_all_command.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/input/system/input_system.h"
#include "violet/log/log.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"

#include "dialog.h"

using namespace edt;
using namespace vlt;

/*static void show_labels_task(void * mem)
{
	auto label_entities = static_cast<std::vector<handle>*>(mem);
	auto & scene = engine::instance().get_current_scene();
	const auto current_entity_ids = scene.get_entity_ids();
	for (const handle entity_id : current_entity_ids)
	{
		if (scene.has_component<editor_component>(entity_id))
		{
			label_entities->emplace_back(scene.load("editor_label.json"));
			add_task(assign_label_task, new assign_label_task_data{ label_entities, entity_id.id }, 0, task_type::write);
		}
	}
}*/

static void load_dialog_task(void * mem)
{
	auto dialog_entities = static_cast<std::vector<handle>*>(mem);
	*dialog_entities = engine::instance().get_current_scene().load("dialog.json");
}

static void remove_dialog_task(void * mem)
{
	auto dialog_entities = make_unique<std::vector<handle>>(mem);
	auto & scene = engine::instance().get_current_scene();
	for (const handle entity_id : *dialog_entities)
		scene.remove_all(entity_id);
}

struct instance final : public cpp_script::instance
{
	instance(cpp_script & script) :
		cpp_script::instance(script),
		dialog_type(None),
		dialog_entities()
	{
		KeyUpMethod::assign(script, KeyUpMethod::Handler::bind<instance, &instance::on_key_up>(this));
		DialogClosedEvent::subscribe(engine::instance(), DialogClosedEvent::subscriber::bind<instance, &instance::onDialogClosed>(this));
	}

	virtual ~instance() override
	{
		KeyUpMethod::remove(script);
		DialogClosedEvent::unsubscribe(engine::instance(), DialogClosedEvent::subscriber::bind<instance, &instance::onDialogClosed>(this));
	}

	void on_key_up(const handle entityId, const window_system::key_event & event)
	{
		// todo: async
		switch (event.code)
		{
		case 'd':
			if ((event.modifiers & key::modifier::M_CTRL) == 0)
			{
				engine::instance().get_system<editor_system>()->execute(create_clear_all_command());
			}
			break;

		case 'z':
			if ((event.modifiers & key::modifier::M_CTRL) != 0)
			{
				engine::instance().get_system<editor_system>()->undo();
			}
			break;

		case ';':
			if (event.modifiers & key::modifier::M_SHIFT)
			{
				// add_task(show_labels_task, &label_entites, 0, task_type::write);
				add_task(load_dialog_task, &dialog_entities, 0, task_type::write);
			}
			break;
		}
	}

	void onDialogClosed(const std::string & input)
	{
		// todo: async (execute, remove_all)
		if (!input.empty())
			engine::instance().get_system<editor_system>()->execute(input);

		add_task(remove_dialog_task, new std::vector<handle>(std::move(dialog_entities)), 0, task_type::write);
		dialog_entities.clear();
	}

private:

	std::vector<handle> dialog_entities;
	std::vector<handle> label_entities;
};

VIOLET_SCRIPT_EXPORT void init(cpp_script & script, std::unique_ptr<cpp_script::instance> & i)
{
	i = std::make_unique<instance>(script);
}

