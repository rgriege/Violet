#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/input/system/InputSystem.h"
#include "violet/input/component/KeyInputComponent.h"
#include "violet/input/component/MouseInputComponent.h"
#include "violet/log/Log.h"
#include "violet/script/ScriptComponent.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/update/component/UpdateComponent.h"
#include "violet/utility/FormattedString.h"

#include "dialog.h"
#include "editor.h"

#include <functional>

using namespace Violet;

ComponentManager::TagMap ms_tagMap = {
    { UpdateComponent::getStaticTag(), EditorComponentWrapper<UpdateComponent>::getStaticTag() },
    { ScriptComponent::getStaticTag(), EditorComponentWrapper<ScriptComponent>::getStaticTag() },
    { KeyInputComponent::getStaticTag(), EditorComponentWrapper<KeyInputComponent>::getStaticTag() },
    { MouseInputComponent::getStaticTag(), EditorComponentWrapper<MouseInputComponent>::getStaticTag() }
};

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_entityIds(std::make_shared<std::vector<Handle>>())
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

    void onKeyUp(const Handle entityId, const unsigned char key)
    {
        if (m_dialog == None)
        {
            switch (key)
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
                    for (const Handle entityId : *m_entityIds)
                        Engine::getInstance().getCurrentScene().removeAll(entityId);
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
                    load(fileName);
                    break;

                case Save:
                    save(fileName);
                    break;
            }
        }
        m_dialog = None;
    }

    void load(const std::string & fileName)
    {
        Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
            [=](ComponentManager & scene)
            {
                auto entityIds = scene.load(fileName.c_str(), ms_tagMap);
                m_entityIds = std::make_shared<std::vector<Handle>>(std::move(entityIds));
            });
    }

    void save(const std::string & fileName)
    {
        Engine::getInstance().getCurrentScene().save(fileName.c_str(), m_entityIds, ms_tagMap);
    }

private:

    enum Dialog
    {
        Load,
        Save,
        None
    };

    Dialog m_dialog = None;
    std::shared_ptr<std::vector<Handle>> m_entityIds;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

