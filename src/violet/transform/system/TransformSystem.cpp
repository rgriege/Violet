// ============================================================================

#include "violet/transform/system/TransformSystem.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/log/Log.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

// ============================================================================

namespace TransformSystemNamespace
{
    typedef std::tuple<const LocalTransformComponent &, const WorldTransformComponent &> TransformEntity;

    void updateWorldTransform(const TransformEntity & entity, std::map<EntityId, Matrix4f> & worldTransformCache, std::vector<TransformEntity> & deferredUpdates);
}

using namespace TransformSystemNamespace;

// ============================================================================

const char * TransformSystem::getStaticLabel()
{
	return "tsfm";
}

// ----------------------------------------------------------------------------

void TransformSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &TransformSystem::init);
}

// ----------------------------------------------------------------------------

void TransformSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.addSystem(std::unique_ptr<System>(new TransformSystem));
		});
}

// ============================================================================

TransformSystem::TransformSystem(TransformSystem && other) :
	System(std::move(other)),
    m_entityWorldTransformCache()
{
}

// ----------------------------------------------------------------------------

TransformSystem::~TransformSystem()
{
}

// ---------------------------------------------------------------------------

void TransformSystem::update(const float /*dt*/)
{
	m_entityWorldTransformCache = std::map<EntityId, Matrix4f>{ { EntityId::ms_invalid, Matrix4f::Identity } };
    std::vector<TransformEntity> deferredUpdates;
	const auto & engine = Engine::getInstance();
	for (const auto & entity : engine.getCurrentScene().getEntityView<LocalTransformComponent, WorldTransformComponent>())
        updateWorldTransform(entity, m_entityWorldTransformCache, deferredUpdates);

    uint32 iteration = 0;
    while (!deferredUpdates.empty() && iteration < 5)
    {
        std::vector<TransformEntity> newDeferredUpdates;
        for (const auto & entity : deferredUpdates)
            updateWorldTransform(entity, m_entityWorldTransformCache, newDeferredUpdates);
        std::swap(deferredUpdates, newDeferredUpdates);
        ++iteration;
    }

    for (const auto & entity : deferredUpdates)
    {
		const auto & localTransformComponent = std::get<0>(entity);
		const auto & worldTransformComponent = std::get<1>(entity);
		const EntityId entityId = localTransformComponent.getEntityId();

        Log::log(FormattedString<128>().sprintf("invalid hierarchy (parent <%d,%d> for <%d,%d>), removing",
            localTransformComponent.m_parentId.getId(),
            localTransformComponent.m_parentId.getVersion(),
            entityId.getId(),
            entityId.getVersion()));

        engine.addWriteTask(engine.getCurrentScene(),
            [=](ComponentManager & scene)
            {
                scene.removeAll(entityId);
            });
    }
}

// ============================================================================

TransformSystem::TransformSystem() :
	System(getStaticLabel()),
	m_entityWorldTransformCache()
{
}

// ============================================================================

void TransformSystemNamespace::updateWorldTransform(const TransformEntity & entity, std::map<EntityId, Matrix4f> & worldTransformCache, std::vector<TransformEntity> & deferredUpdates)
{
	const auto & engine = Engine::getInstance();
    const auto & localTransformComponent = std::get<0>(entity);
    const auto & worldTransformComponent = std::get<1>(entity);
    const EntityId entityId = localTransformComponent.getEntityId();

    const auto it = worldTransformCache.find(localTransformComponent.m_parentId);
    if (it != worldTransformCache.end())
    {
        const Matrix4f & parentWorldTransform = it != worldTransformCache.end() ? it->second : Matrix4f::Identity;
        const Matrix4f & worldTransform = worldTransformCache[entityId] = parentWorldTransform * localTransformComponent.m_transform;
        if (worldTransformComponent.m_transform != worldTransform)
        {
            engine.addWriteTask(*engine.getCurrentScene().getPool<WorldTransformComponent>(),
                [entityId, &worldTransformCache](ComponentPool & pool)
                {
                    pool.get<WorldTransformComponent>(entityId)->m_transform = worldTransformCache[entityId];
                });
        }
    }
    else
        deferredUpdates.emplace_back(entity);
}

// ============================================================================
