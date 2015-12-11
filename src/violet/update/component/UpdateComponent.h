#ifndef VIOLET_UpdateComponent_H
#define VIOLET_UpdateComponent_H

#include "violet/component/Component.h"
#include "violet/task/Thread.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API UpdateComponent : public ComponentBase<UpdateComponent>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		UpdateComponent(EntityId entityId);
		UpdateComponent(EntityId entityId, Deserializer & deserializer);
		UpdateComponent(UpdateComponent && other);
	};

	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, UpdateComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const UpdateComponent & component);
}

#endif