#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/utility/Factory.h"

namespace Violet
{
	class VIOLET_API EntityFactory : public Factory<const char *, Entity(Deserializer &)>
	{
	public:

		static EntityFactory & getInstance();

	private:

		EntityFactory();
		~EntityFactory();

		EntityFactory(const EntityFactory &) = delete;
		EntityFactory & operator=(const EntityFactory &) = delete;
	};
}

#endif