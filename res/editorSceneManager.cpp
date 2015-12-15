#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/graphics/component/ColorComponent.h"
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
        m_entityIds()
    {
        using namespace std::placeholders;
        KeyUpMethod::assign(script, KeyUpMethod::Handler::bind<Instance, &Instance::onKeyUp>(this));
        DialogClosedEvent::subscribe(Engine::getInstance(), DialogClosedEvent::Subscriber::bind<Instance, &Instance::onDialogClosed>(this));
        QuitEvent::subscribe(Engine::getInstance(), QuitEvent::Subscriber::bind<Instance, &Instance::onQuit>(this));
    }

    virtual ~Instance() override
    {
        KeyUpMethod::remove(m_script);
        DialogClosedEvent::unsubscribe(Engine::getInstance(), DialogClosedEvent::Subscriber::bind<Instance, &Instance::onDialogClosed>(this));
        QuitEvent::unsubscribe(Engine::getInstance(), QuitEvent::Subscriber::bind<Instance, &Instance::onQuit>(this));
    }

private:

    void onKeyUp(const EntityId entityId, const unsigned char key)
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
                    for (const EntityId entityId : m_entityIds)
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

    void onQuit()
    {
        const auto & engine = Engine::getInstance();
        for (auto const entityId : m_entityIds)
        {
            engine.addWriteTask(*engine.getCurrentScene().getPool<ScriptComponent>(),
                [=](ComponentPool & pool)
                {
                    pool.remove(entityId);
                });
            engine.addWriteTask(*engine.getCurrentScene().getPool<MouseInputComponent>(),
                [=](ComponentPool & pool)
                {
                    pool.remove(entityId);
                });
        }
    }

    void load(const std::string & fileName)
    {
        Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
            [=](ComponentManager & scene)
            {
                auto entityIds = scene.load(fileName.c_str(), ms_tagMap);
                for (const auto entityId : entityIds)
                    addEditBehavior(scene, entityId);
                std::copy(entityIds.begin(), entityIds.end(), std::back_inserter(m_entityIds));
            });
    }

    static void addEditBehavior(const ComponentManager & scene, const EntityId entityId)
    {
        Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
            [&scene, entityId]()
            {
                if (scene.hasComponent<ColorComponent>(entityId))
                {
                    Engine::getInstance().addWriteTask(*scene.getPool<ScriptComponent>(),
                        [=](ComponentPool & pool)
                        {
                            pool.create<ScriptComponent>(entityId, "editEntity.dll");
                        });
                    addMouseInput(scene, entityId, scene.getComponent<ColorComponent>(entityId)->m_mesh->getPolygon());
                }
            }), Thread::Window);
    }

    static void addMouseInput(const ComponentManager & scene, const EntityId entityId, Polygon && poly)
    {
        Engine::getInstance().addWriteTask(*scene.getPool<MouseInputComponent>(),
            [=](ComponentPool & pool) mutable
            {
                pool.create<MouseInputComponent>(entityId, std::move(poly));
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
    std::vector<EntityId> m_entityIds;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

