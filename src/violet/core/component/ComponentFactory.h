#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/utility/Factory.h"

namespace Violet
{
	class VIOLET_API ComponentFactory : public Factory<const char *, void(Entity &, Deserializer &)>
	{
	public:

		static ComponentFactory & getInstance();

	private:

		ComponentFactory();
		ComponentFactory(const ComponentFactory &) = delete;
		ComponentFactory & operator=(const ComponentFactory &) = delete;
	};
}

#endif