#ifndef VIOLET_UpdateComponent_H
#define VIOLET_UpdateComponent_H

#include "violet/component/Component.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API UpdateComponent : public ComponentBase<UpdateComponent>
	{
	public:

		static Tag getStaticTag();

	public:

		UpdateComponent(Entity & owner);
		UpdateComponent(Entity & owner, Deserializer & deserializer);
		UpdateComponent(UpdateComponent && other);
	};

	Deserializer & operator>>(Deserializer & deserializer, UpdateComponent & component);
	Serializer & operator<<(Serializer & serializer, const UpdateComponent & component);
}

#endif