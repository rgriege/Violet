#include "violet/core/transform/TransformSystem.h"

#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

namespace TransformSystemNamespace
{
	const char * const ms_componentLabel = "tsfm";

	TransformSystem * ms_transformSystem = nullptr;
}

using namespace TransformSystemNamespace;

bool TransformSystem::init(ComponentFactory & factory)
{
	if (ms_transformSystem == nullptr)
	{
		ms_transformSystem = new TransformSystem();
		factory.assign(ms_componentLabel, &TransformSystem::create);
		return true;
	}

	return false;
}

void TransformSystem::update(float /*dt*/)
{
}

void TransformSystem::create(Entity & entity, Deserializer & deserializer)
{
	auto segment = deserializer.enterSegment(ms_componentLabel);
	ms_transformSystem->m_entityComponentMap.emplace(entity.m_id, ms_transformSystem->m_components.size());
	ms_transformSystem->m_components.emplace_back(entity, *segment);
}

TransformComponent & TransformSystem::fetch(const Entity & entity)
{
	return ms_transformSystem->m_components[ms_transformSystem->m_entityComponentMap[entity.m_id]];
}