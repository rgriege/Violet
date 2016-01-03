#include "editor/CppScriptExports.h"
#include "editor/EditorSystem.h"
#include "editor/component/EditorComponent.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/log/Log.h"
#include "violet/math/Polygon.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/script/ScriptComponent.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/transform/Transform.h"
#include "violet/utility/FormattedString.h"

using namespace edt;
using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_dragging(false),
        m_selected(false),
        m_mouseDownPos(),
        m_startDragPos()
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
            const auto & engine = Engine::getInstance();

            m_dragging = true;
            m_mouseDownPos = event.position;
            if (engine.getCurrentScene().hasComponent<LocalTransformComponent>(entityId))
            {
                const auto * ltc = engine.getCurrentScene().getComponent<LocalTransformComponent>(entityId);
                m_startDragPos = Transform::getPosition(ltc->m_transform);
            }
            else
            {
                const auto * wtc = engine.getCurrentScene().getComponent<WorldTransformComponent>(entityId);
                m_startDragPos = Transform::getPosition(wtc->m_transform);
            }

            if (!m_selected)
            {
                m_selected = true;
                addMutationHandle(entityId);
            }

            return InputResult::Block;
        }

        return InputResult::Pass;
    }

    void onMouseMove(const EntityId entityId, const InputSystem::MouseMotionEvent & event)
    {
        if (m_dragging)
        {
            const auto & engine = Engine::getInstance();
            const auto & newPosition = m_startDragPos + event.to - m_mouseDownPos;
            if (engine.getCurrentScene().hasComponent<LocalTransformComponent>(entityId))
                move<LocalTransformComponent>(entityId, newPosition);
            else
                move<WorldTransformComponent>(entityId, newPosition);
        }
    }

    InputResult onMouseUp(const EntityId entityId, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            if (m_dragging)
            {
                const auto & engine = Engine::getInstance();
                m_dragging = false;
                const auto * ec = engine.getCurrentScene().getComponent<EditorComponent>(entityId);
                const Vec2f pos = m_startDragPos + event.position - m_mouseDownPos;
                engine.addWriteTask(*engine.getSystem<EditorSystem>(),
                    [=](EditorSystem & editor)
                    {
                        editor.execute(createMoveToCommand(ec->m_editId, pos));
                    });
                m_startDragPos = Vec2f::ZERO;
                m_mouseDownPos = Vec2f::ZERO;
            }
            return InputResult::Block;
        }

        return InputResult::Pass;
    }

    template <typename ComponentType>
    void move(const EntityId entityId, const Vec2f & position)
    {
        const auto & engine = Engine::getInstance();
        engine.addWriteTask(*engine.getCurrentScene().getPool<ComponentType>(),
            [=](ComponentPool & pool)
            {
                auto & transform = pool.get<ComponentType>(entityId)->m_transform;
                Transform::setPosition(transform, position);
            });
    }

    void addMutationHandle(const EntityId entityId)
    {
        const auto & engine = Engine::getInstance();
        if (!engine.getCurrentScene().hasComponent<LocalTransformComponent>(entityId))
        {
            engine.addWriteTask(*engine.getCurrentScene().getPool<LocalTransformComponent>(),
                [=](ComponentPool & pool)
                {
                    pool.create<LocalTransformComponent>(entityId, EntityId::ms_invalid, Matrix3f::Identity);
                });
        }

        engine.addWriteTask(engine.getCurrentScene(),
            [entityId](ComponentManager & scene)
            {
                const EntityId handleId = scene.load("mutationHandle.json").front();

                Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
                    [entityId, handleId]()
                    {
                        const auto & engine = Engine::getInstance();
                        const auto & pool = engine.getCurrentScene().getPool<ColorComponent>();
                        const auto & poly = pool->get<ColorComponent>(entityId)->m_mesh->getPolygon();
                        const Vec2f & offset = poly.getBoundingBox().getMaximum();

                        engine.addWriteTask(*engine.getCurrentScene().getPool<LocalTransformComponent>(),
                            [entityId, handleId, offset](ComponentPool & pool)
                            {
                                auto * ltc = pool.get<LocalTransformComponent>(handleId);
                                if (ltc != nullptr)
                                {
                                    ltc->m_parentId = entityId;
                                    Transform::setPosition(ltc->m_transform, offset);
                                }
                            });
                    }), ColorComponent::getStaticThread());
            });
    }

private:

    bool m_dragging;
    bool m_selected;
    Vec2f m_mouseDownPos;
    Vec2f m_startDragPos;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

