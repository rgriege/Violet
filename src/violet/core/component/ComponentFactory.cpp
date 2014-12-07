#include "violet/core/component/ComponentFactory.h"

using namespace Violet;

ComponentFactory & ComponentFactory::getInstance()
{
	static ComponentFactory ms_instance;
	return ms_instance;
}

ComponentFactory::ComponentFactory() :
	Factory<const char *, void(Entity &, Deserializer &)>()
{
}