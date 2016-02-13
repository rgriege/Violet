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

#include "dialog.h"

using namespace edt;
using namespace vlt;

class instance final : public cpp_script::instance
{
public:

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

private:

    void on_key_up(const handle entityId, const window_system::key_event & event)
    {
        const auto & ngn = engine::instance();
        if (m_dialog == None)
        {
            switch (event.code)
            {
                case 'o':
					ngn.add_write_task(ngn.get_current_scene(),
                        [=](scene & scene)
                        {
							m_dialogEntities = scene.load("dialog.json");
                        });
                    m_dialog = Load;
                    break;

                case 's':
					ngn.add_write_task(ngn.get_current_scene(),
                        [=](scene & scene)
                        {
							m_dialogEntities = scene.load("dialog.json");
                        });
                    m_dialog = Save;
                    break;

                case 'c':
                    if ((event.modifiers & key::modifier::M_CTRL) == 0)
                    {
						ngn.add_write_task(*ngn.get_system<editor_system>(),
                            [](editor_system & editor)
                            {
                                editor.execute(create_clear_all_command());
                            });
                    }
                    else
                        printf("%x - %x - %x\n", event.modifiers, key::modifier::M_CTRL, event.modifiers & key::modifier::M_CTRL);
                    break;

                case 'z':
                    if ((event.modifiers & key::modifier::M_CTRL) != 0)
                    {
                        ngn.add_write_task(*ngn.get_system<editor_system>(),
                            [](editor_system & editor)
                            {
                                editor.undo();
                            });
                    }
                    break;

                case key::Return:
                    ngn.add_write_task(ngn.get_current_scene(),
                        [=](scene & s)
                        {
							m_dialogEntities = s.load("dialog.json");
                        });
                    m_dialog = Command;
                    break;
            }
        }
    }

    void onDialogClosed(const std::string & input)
    {
		const auto & ngn = engine::instance();
        if (!input.empty())
        {
            switch (m_dialog)
            {
                case Command:
                    ngn.add_write_task(*ngn.get_system<editor_system>(),
                        [=](editor_system & editor)
                        {
                            editor.execute(input);
                        });
                    break;

                case Load:
                    ngn.add_write_task(*ngn.get_system<editor_system>(),
                        [=](editor_system & editor)
                        {
                            editor.execute(create_open_command(input));
                        });
                    break;

                case Save:
                    ngn.add_write_task(*ngn.get_system<editor_system>(),
                        [=](editor_system & editor)
                        {
                            editor.execute(create_save_all_command(input));
                        });
                    break;
            }
        }
        m_dialog = None;
		for (const handle entityId : m_dialogEntities)
		{
			ngn.add_write_task(ngn.get_current_scene(),
				[=](scene & s)
				{
					s.remove_all(entityId);
				});
		}
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

