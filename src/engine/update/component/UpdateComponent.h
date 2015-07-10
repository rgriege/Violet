#ifndef VIOLET_UpdateComponent_H
#define VIOLET_UpdateComponent_H

#include "engine/component/Component.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API UpdateComponent : public ComponentBase<UpdateComponent>
	{
	public:

		static Tag getTag();

	public:

		UpdateComponent(const Entity & owner);
		UpdateComponent(const Entity & owner, Deserializer & deserializer);
	};

	Deserializer & operator>>(Deserializer & deserializer, UpdateComponent & component);
	Serializer & operator<<(Serializer & serializer, const UpdateComponent & component);
}

#endif