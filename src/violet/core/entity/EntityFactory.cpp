#include "violet/core/entity/EntityFactory.h"

#include "violet/core/Engine.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"

#include <iostream>

using namespace Violet;

namespace EntityFactoryNamespace
{
	const char * ms_entityLabel = "ntty";
}

using namespace EntityFactoryNamespace;

EntityFactory::EntityFactory() :
	Factory<const char *, void (Deserializer &, SceneInitContext &)>(),
	m_freeList()
{
	assign(ms_entityLabel, std::bind(&EntityFactory::createFromComponentList, this, std::placeholders::_1, std::placeholders::_2));
}

EntityFactory::~EntityFactory()
{
	remove(ms_entityLabel);
}

Entity EntityFactory::createNew()
{
	return Entity(m_freeList.reserve());
}

void EntityFactory::free(const Entity & entity)
{
	return m_freeList.free(entity.getId());
}

void EntityFactory::createFromComponentList(Deserializer & deserializer, SceneInitContext & initContext)
{
	auto entitySegment = deserializer.enterSegment(ms_entityLabel);
	uint32 id = entitySegment->getUint("id");
	if (!m_freeList.reserve(id))
	{
		uint32 const failedId = id;
		id = m_freeList.reserve();
		std::cout << "Failed to create entity with id " << failedId << ", using " << id << " instead." << std::endl;
	}
	Entity entity(id);
	while (*entitySegment)
		initContext.createComponent(entitySegment->nextLabel(), entity, *entitySegment);
}