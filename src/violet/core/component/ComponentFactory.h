#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include "violet/core/utility/Factory.h"

namespace Violet
{
	class Deserializer;
	class Entity;

	class VIOLET_API ComponentFactory : public Factory<const char *, void(Entity &, Deserializer &)> {};
}

#endif