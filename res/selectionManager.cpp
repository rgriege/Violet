#include "editor/EditorSystem.h"
#include "editor/CppScriptExports.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/log/Log.h"
#include "violet/math/AABB.h"
#include "violet/math/Polygon.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/transform/Transform.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/utility/FormattedString.h"

using namespace edt;
using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script)
    {
        EntityDeselectedEvent::subscribe(Engine::getInstance(), EntityDeselectedEvent::Subscriber::bind<Instance, &Instance::onEntityDeselected>(this));
        EntitySelectedEvent::subscribe(Engine::getInstance(), EntitySelectedEvent::Subscriber::bind<Instance, &Instance::onEntitySelected>(this));
    }

    virtual ~Instance() override
    {
        EntityDeselectedEvent::unsubscribe(Engine::getInstance(), EntityDeselectedEvent::Subscriber::bind<Instance, &Instance::onEntityDeselected>(this));
        EntitySelectedEvent::unsubscribe(Engine::getInstance(), EntitySelectedEvent::Subscriber::bind<Instance, &Instance::onEntitySelected>(this));
    }

private:

    void onEntityDeselected(const EntityId entityId)
    {
		Log::log(FormattedString<128>().sprintf("<%d> deselected", entityId.getId()));
        const auto & engine = Engine::getInstance();
        const auto & editor = *engine.getSystem<EditorSystem>();
        const auto & selectedEntities = editor.getSelectedEntities();
        if (!selectedEntities.empty())
        {
            /*for (const auto & entity : selectedEntities)
                onEntitySelected(entity);*/
        }
        else
        {
            // engine.getCurrentScene().removeAll(m_minimumBoxId);
            // engine.getCurrentScene().removeAll(m_maximumBoxId);
            m_minimumBoxId = EntityId();
            m_maximumBoxId = EntityId();
            m_selectionBox = AABB();
        }
    }

    void onEntitySelected(const EntityId entityId)
    {
		Log::log(FormattedString<128>().sprintf("<%d> selected", entityId.getId()));
        const auto & engine = Engine::getInstance();
        engine.addWriteTask(engine.getSystem<EditorSystem>()->getScene(),
            [=](ComponentManager & scene)
            {
				const auto * cc = scene.getComponent<ColorComponent>(entityId);
                AABB box = cc->m_mesh->getPolygon().getBoundingBox();
                const Vec2f pos = Transform::getPosition(scene.getComponent<WorldTransformComponent>(entityId)->m_transform);
                box.translate(pos);
                if (m_selectionBox.empty())
                {
                    m_selectionBox = box;
                    // addMutationHandle(m_minimumBoxId);
                    // addMutationHandle(m_maximumBoxId);
                }
                else
                {
                    m_selectionBox.extend(box);
                    // moveMutationHandle(m_minimumBoxId);
                    // moveMutationHandle(m_maximumBoxId);
                }
            }, ColorComponent::getStaticThread());
    }

    void addMutationHandle(EntityId & entityId)
    {
        const auto & engine = Engine::getInstance();
        engine.addWriteTask(engine.getCurrentScene(),
            [&entityId, this](ComponentManager & scene)
            {
                entityId = scene.load("mutationHandle.json").front();
                moveMutationHandle(entityId);
            });
    }

    void moveMutationHandle(const EntityId entityId)
    {
        const auto & engine = Engine::getInstance();
        const Vec2f & offset = entityId == m_minimumBoxId
            ? m_selectionBox.getMinimum()
            : m_selectionBox.getMaximum();
        engine.addWriteTask(*engine.getCurrentScene().getPool<WorldTransformComponent>(),
            [=](ComponentPool & pool)
            {
                Transform::setPosition(pool.get<WorldTransformComponent>(entityId)->m_transform, offset);
            });
    }

private:

    AABB m_selectionBox;
    EntityId m_minimumBoxId;
    EntityId m_maximumBoxId;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

