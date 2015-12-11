#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/input/system/InputSystem.h"
#include "violet/log/Log.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/script/ScriptComponent.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/transform/Transform.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_dragging(false)
    {
        MouseDownMethod::assign(m_script, MouseDownMethod::Handler::bind<Instance, &Instance::onMouseDown>(this));
        MouseMoveMethod::assign(m_script, MouseMoveMethod::Handler::bind<Instance, &Instance::onMouseMove>(this));
        MouseUpMethod::assign(m_script, MouseUpMethod::Handler::bind<Instance, &Instance::onMouseUp>(this));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(m_script);
        MouseMoveMethod::remove(m_script);
        MouseUpMethod::remove(m_script);
    }

private:

    InputResult onMouseDown(const EntityId entityId, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            createComponentMenu(entityId);
            m_dragging = true;
            return InputResult::Block;
        }

        return InputResult::Pass;
    }

    void onMouseMove(const EntityId entityId, const InputSystem::MouseMotionEvent & event)
    {
        if (m_dragging)
        {
            const auto & engine = Engine::getInstance();
            const auto & newPosition = event.to;
            engine.addWriteTask(*engine.getCurrentScene().getPool<WorldTransformComponent>(),
                [=](ComponentPool & pool)
                {
                    auto & transform = pool.get<WorldTransformComponent>(entityId)->m_transform;
                    Transform::setPosition(transform, newPosition);
                });
        }
    }

    InputResult onMouseUp(const EntityId entityId, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            m_dragging = false;
            return InputResult::Block;
        }

        return InputResult::Pass;
    }

    void createComponentMenu(const EntityId entityId)
    {
        /*const auto & engine = Engine::getInstance();
        const auto & menu = engine.getCurrentScene().getEntity(EntityId(4000, 0));
        if (menu != nullptr)
        {
            if (entity.hasComponent<TransformComponent>())
            {
                engine.addWriteTask(*menu, [&](Entity & m)
                    {
                        const auto & deserializer = FileDeserializerFactory::getInstance().create("editTransform.json");
                        if (deserializer != nullptr)
                            m.addChild(*deserializer);

                        const auto & engine = Engine::getInstance();
                        engine.addWriteTask(engine.getCurrentScene(), [&](Scene & scene)
                            {
                                EntitySelectedEvent::emit(scene, entity);
                            });
                    });
            }
        }*/
    }

private:

    bool m_dragging;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

