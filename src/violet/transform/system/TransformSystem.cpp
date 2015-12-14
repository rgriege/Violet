// ============================================================================

#include "violet/transform/system/TransformSystem.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"

using namespace Violet;

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
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

TransformSystem::~TransformSystem()
{
}

// ---------------------------------------------------------------------------

void TransformSystem::update(const float /*dt*/)
{
	m_entityWorldTransformCache.clear();
	const auto & engine = Engine::getInstance();
	for (const auto & entity : engine.getCurrentScene().getEntityView<LocalTransformComponent, WorldTransformComponent>())
	{
		const auto & localTransformComponent = std::get<0>(entity);
		const auto & worldTransformComponent = std::get<1>(entity);
		const EntityId entityId = localTransformComponent.getEntityId();

		if (!localTransformComponent.m_parentId.isValid())
		{
			m_entityWorldTransformCache[entityId] = localTransformComponent.m_transform;
			if (localTransformComponent.m_transform != worldTransformComponent.m_transform)
			{
				engine.addWriteTask(*engine.getCurrentScene().getPool<WorldTransformComponent>(),
					[=](ComponentPool & pool)
					{
						pool.get<WorldTransformComponent>(entityId)->m_transform = m_entityWorldTransformCache[entityId];
					});
			}
		}
		else
		{
			const Matrix3f & parentWorldTransform = m_entityWorldTransformCache[localTransformComponent.m_parentId];
			const Matrix3f & worldTransform = m_entityWorldTransformCache[entityId] = parentWorldTransform * localTransformComponent.m_transform;
			if (worldTransformComponent.m_transform != worldTransform)
			{
				engine.addWriteTask(*engine.getCurrentScene().getPool<WorldTransformComponent>(),
					[=](ComponentPool & pool)
					{
						pool.get<WorldTransformComponent>(entityId)->m_transform = m_entityWorldTransformCache[entityId];
					});
			}
		}
	}
}

// ============================================================================

TransformSystem::TransformSystem() :
	System(getStaticLabel()),
	m_entityWorldTransformCache()
{
}

// ============================================================================