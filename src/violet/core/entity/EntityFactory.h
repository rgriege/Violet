#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "violet/core/utility/Factory.h"

namespace Violet
{
	class Deserializer;
	class SceneInitContext;

	class VIOLET_API EntityFactory : public Factory<const char *, void(Deserializer &, SceneInitContext &)>
	{
	public:

		EntityFactory();
		~EntityFactory();
	};
}

#endif