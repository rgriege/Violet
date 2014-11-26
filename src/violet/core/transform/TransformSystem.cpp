#include "violet/core/transform/TransformSystem.h"

#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

namespace TransformSystemNamespace
{
	const char * const ms_componentLabel = "tsfm";

	TransformSystem * ms_transformSystem = nullptr;
}

using namespace TransformSystemNamespace;

bool TransformSystem::init()
{
	if (ms_transformSystem == nullptr)
	{
		ms_transformSystem = new TransformSystem();
		return true;
	}

	return false;
}

void TransformSystem::update(float /*dt*/)
{
}

void TransformSystem::create(Entity & entity, Deserializer & deserializer)
{
	deserializer.enterSegment(ms_componentLabel);
	ms_transformSystem->m_entityComponentMap.emplace(entity.id, ms_transformSystem->m_components.size());
	ms_transformSystem->m_components.emplace_back(entity, deserializer);
	deserializer.leaveSegment();
}

TransformComponent & TransformSystem::fetch(const Entity & entity)
{
	return ms_transformSystem->m_components[ms_transformSystem->m_entityComponentMap[entity.id]];
}