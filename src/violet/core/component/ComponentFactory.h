#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/utility/Factory.h"

#include <string>

namespace Violet
{
	class ComponentFactory : private Factory<std::string, void(Entity &, Deserializer &)>
	{
	public:

		void create(Entity & entity, Deserializer & deserializer);
		void assign(const char * label, const Producer & producer);
	};
}

#endif