#include "violet/core/component/ComponentFactory.h"

using namespace Violet;

void ComponentFactory::create(Entity & entity, Deserializer & deserializer)
{
	const char * label = deserializer.nextLabel();
	Factory::create(label, entity, deserializer);
}

void ComponentFactory::assign(const char * label, const Producer & producer)
{
	Factory::assign(label, producer);
}
