#ifndef VIOLET_UpdateComponent_H
#define VIOLET_UpdateComponent_H

#include "engine/component/Component.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API UpdateComponent : public Component<UpdateComponent>
	{
	public:

		static Tag getTypeId();

	public:

		UpdateComponent(Entity entity);
		UpdateComponent(Entity entity, Deserializer & deserializer);
	};

	Deserializer & operator>>(Deserializer & deserializer, UpdateComponent & component);
	Serializer & operator<<(Serializer & serializer, const UpdateComponent & component);
}

#endif