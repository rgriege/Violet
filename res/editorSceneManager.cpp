#include "editor/Editor.h"
#include "editor/CppScriptExports.h"
#include "editor/command/file/OpenCommand.h"
#include "editor/command/file/SaveAllCommand.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/input/system/InputSystem.h"
#include "violet/log/Log.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/script/ScriptUtilities.h"
#include "violet/utility/FormattedString.h"

#include "dialog.h"

#include <functional>

using namespace edt;
using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script)
    {
        using namespace std::placeholders;
        KeyUpMethod::assign(script, KeyUpMethod::Handler::bind<Instance, &Instance::onKeyUp>(this));
        DialogClosedEvent::subscribe(Engine::getInstance(), DialogClosedEvent::Subscriber::bind<Instance, &Instance::onDialogClosed>(this));
    }

    virtual ~Instance() override
    {
        KeyUpMethod::remove(m_script);
        DialogClosedEvent::unsubscribe(Engine::getInstance(), DialogClosedEvent::Subscriber::bind<Instance, &Instance::onDialogClosed>(this));
    }

private:

    void onKeyUp(const EntityId entityId, const WindowSystem::KeyEvent & event)
    {
        if (m_dialog == None)
        {
            switch (event.code)
            {
                case 'o':
                    Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
                        [](ComponentManager & scene)
                        {
                            scene.load("dialog.json");
                        });
                    m_dialog = Load;
                    break;

                case 's':
                    Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
                        [](ComponentManager & scene)
                        {
                            scene.load("dialog.json");
                        });
                    m_dialog = Save;
                    break;

                case 'c':
                    if ((event.modifiers & Key::Modifier::M_CTRL) == 0)
                        Editor::execute(createClearAllCommand());
                    else
                        printf("%x - %x - %x\n", event.modifiers, Key::Modifier::M_CTRL, event.modifiers & Key::Modifier::M_CTRL);
                    break;

				case 'z':
					if ((event.modifiers & Key::Modifier::M_CTRL) != 0)
						Editor::undo();
                    break;
            }
        }
    }

    void onDialogClosed(const std::string & fileName)
    {
        if (!fileName.empty())
        {
            switch (m_dialog)
            {
                case Load:
                    Editor::execute(createOpenCommand(fileName));
					// Editor::execute(std::make_unique<OpenCommand>(fileName));
                    break;

                case Save:
					Editor::execute(std::make_unique<SaveAllCommand>(fileName));
                    break;
            }
        }
        m_dialog = None;
    }

private:

    enum Dialog
    {
        Load,
        Save,
        None
    };

    Dialog m_dialog = None;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

