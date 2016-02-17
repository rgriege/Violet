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
		m_dialog(None),
		m_dialogEntities()
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
        if (m_dialog == None)
        {
            switch (event.code)
            {
                case 'o':
					add_task(load_dialog_task, &m_dialogEntities, 0, task_type::write);
                    m_dialog = Load;
                    break;

                case 's':
					add_task(load_dialog_task, &m_dialogEntities, 0, task_type::write);
                    m_dialog = Save;
                    break;

                case 'c':
                    if ((event.modifiers & key::modifier::M_CTRL) == 0)
                    {
						engine::instance().get_system<editor_system>()->execute(create_clear_all_command());
                    }
                    else
                        printf("%x - %x - %x\n", event.modifiers, key::modifier::M_CTRL, event.modifiers & key::modifier::M_CTRL);
                    break;

                case 'z':
                    if ((event.modifiers & key::modifier::M_CTRL) != 0)
                    {
						engine::instance().get_system<editor_system>()->undo();
                    }
                    break;

                case key::Return:
					add_task(load_dialog_task, &m_dialogEntities, 0, task_type::write);
                    m_dialog = Command;
                    break;
            }
        }
    }

    void onDialogClosed(const std::string & input)
    {
		// todo: async (execute, remove_all)
        if (!input.empty())
        {
            switch (m_dialog)
            {
                case Command:
					engine::instance().get_system<editor_system>()->execute(input);
                    break;

                case Load:
					engine::instance().get_system<editor_system>()->execute(create_open_command(input));
                    break;

                case Save:
					engine::instance().get_system<editor_system>()->execute(create_save_all_command(input));
                    break;
            }
        }
        m_dialog = None;
		add_task(remove_dialog_task, new std::vector<handle>(std::move(m_dialogEntities)), 0, task_type::write);
		m_dialogEntities.clear();
    }

private:

    enum Dialog
    {
        Command,
        Load,
        Save,
        None
    };

    Dialog m_dialog = None;
	std::vector<handle> m_dialogEntities;
};

VIOLET_SCRIPT_EXPORT void init(cpp_script & script, std::unique_ptr<cpp_script::instance> & i)
{
    i = std::make_unique<instance>(script);
}

