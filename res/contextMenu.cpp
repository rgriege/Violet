#include "violet/Engine.h"
#include "violet/handle/HandleComponent.h"
#include "violet/input/InputResult.h"
#include "violet/input/system/InputSystem.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/TransformComponent.h"

#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_menuHandle(Handle::ms_invalid)
    {
        using namespace std::placeholders;
        MouseDownMethod::assign(script, std::bind(&Instance::onMouseDown, this, _1, _2, _3));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(m_script);
    }

private:

    InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Right)
        {
            if (activeMenu())
            {
                removeMenu(engine);
                createMenu(engine, event.position);
            }
            else
                createMenu(engine, event.position);
        }
        else if (activeMenu())
            removeMenu(engine);
        else
            return InputResult::Pass;

        return InputResult::Block;
    }

    bool activeMenu()
    {
        return m_menuHandle.isValid();
    }

    void createMenu(const Engine & engine, const Vec2f & position)
    {
        engine.addWriteTask(engine.getCurrentScene().getRoot(), [position, this](Entity & root)
            {
                auto deserializer = FileDeserializerFactory::getInstance().create("block.json");
                if (deserializer != nullptr && *deserializer)
                {
                    Entity & menu = root.addChild(*deserializer);
                    auto transformComponent = menu.getComponent<TransformComponent>();
                    if (transformComponent != nullptr)
                    {
                        transformComponent->m_transform[0][2] = position.x;
                        transformComponent->m_transform[1][2] = position.y;
                    }
                    menu.addComponent<HandleComponent>();
                    m_menuHandle = menu.getComponent<HandleComponent>()->getHandle();
                }
            });
    }

    void removeMenu(const Engine & engine)
    {
        auto const & menu = engine.getCurrentScene().getEntity(m_menuHandle);
        if (menu != nullptr)
        {
            engine.addWriteTask(*menu, [](Entity & menu)
                {
                    menu.removeFromParent();
                });
        }
        m_menuHandle = Handle::ms_invalid;
    }

private:

    Handle m_menuHandle;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

