#include "engine/entity/EntityFactory.h"

#include "engine/Engine.h"
#include "engine/entity/Entity.h"
#include "engine/serialization/Deserializer.h"

#include <iostream>

using namespace Violet;

namespace EntityFactoryNamespace
{
	const char * ms_entityLabel = "ntty";
}

using namespace EntityFactoryNamespace;

EntityFactory::EntityFactory() :
	Factory<const char *, void (Deserializer &, Engine &)>(),
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

std::vector<uint32> EntityFactory::getEntities() const
{
	return m_freeList.getUsed();
}

void EntityFactory::createFromComponentList(Deserializer & deserializer, Engine & engine)
{
	auto entitySegment = deserializer.enterSegment(ms_entityLabel);
	uint32 id;
	if (entitySegment->nextLabel() == std::string("id"))
	{
		id = entitySegment->getUint("id");
		if (!m_freeList.reserve(id))
		{
			uint32 const failedId = id;
			id = m_freeList.reserve();
			std::cout << "Failed to create entity with id " << failedId << ", using " << id << " instead." << std::endl;
		}
	}
	else
		id = m_freeList.reserve();
	Entity entity(id);
	while (*entitySegment)
		engine.getComponentFactory().create(entitySegment->nextLabel(), entity, *entitySegment);
}