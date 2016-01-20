#ifndef VIOLET_UpdateComponent_H
#define VIOLET_UpdateComponent_H

#include "violet/component/Component.h"
#include "violet/task/Thread.h"

namespace Violet
{
	class ComponentDeserializer;
	class Serializer;

	class VIOLET_API UpdateComponent : public ComponentBase<UpdateComponent, 0>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		UpdateComponent(EntityId entityId);
		UpdateComponent(EntityId entityId, ComponentDeserializer & deserializer);
		UpdateComponent(UpdateComponent && other);
	};

	VIOLET_API ComponentDeserializer & operator>>(ComponentDeserializer & deserializer, UpdateComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const UpdateComponent & component);
}

#endif