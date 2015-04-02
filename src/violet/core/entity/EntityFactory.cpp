#include "violet/core/entity/EntityFactory.h"

#include "violet/core/component/ComponentFactory.h"

using namespace Violet;

namespace EntityFactoryNamespace
{
	const char * ms_entityLabel = "ntty";

	Entity createFromComponentList(Deserializer & deserializer);
}

using namespace EntityFactoryNamespace;

EntityFactory & EntityFactory::getInstance()
{
	static EntityFactory s_instance;
	return s_instance;
}

EntityFactory::EntityFactory() :
	Factory<const char *, Entity(Deserializer &)>()
{
	assign(ms_entityLabel, createFromComponentList);
}

EntityFactory::~EntityFactory()
{
	remove(ms_entityLabel);
}

Entity EntityFactoryNamespace::createFromComponentList(Deserializer & deserializer)
{
	auto entitySegment = deserializer.enterSegment(ms_entityLabel);
	Entity entity(entitySegment->getUint("id"));
	while (*entitySegment)
		ComponentFactory::getInstance().create(entitySegment->nextLabel(), entity, *entitySegment);
	return entity;
}