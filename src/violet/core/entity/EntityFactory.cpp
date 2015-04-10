#include "violet/core/entity/EntityFactory.h"

#include "violet/core/Engine.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

namespace EntityFactoryNamespace
{
	const char * ms_entityLabel = "ntty";

	void createFromComponentList(Deserializer & deserializer, SceneInitContext & initContext);
}

using namespace EntityFactoryNamespace;

EntityFactory::EntityFactory() :
	Factory<const char *, void (Deserializer &, SceneInitContext &)>()
{
	assign(ms_entityLabel, createFromComponentList);
}

EntityFactory::~EntityFactory()
{
	remove(ms_entityLabel);
}

void EntityFactoryNamespace::createFromComponentList(Deserializer & deserializer, SceneInitContext & initContext)
{
	auto entitySegment = deserializer.enterSegment(ms_entityLabel);
	Entity entity(entitySegment->getUint("id"));
	while (*entitySegment)
		initContext.createComponent(entitySegment->nextLabel(), entity, *entitySegment);
}