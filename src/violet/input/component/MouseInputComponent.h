#ifndef VIOLET_MouseInputComponent_H
#define VIOLET_MouseInputComponent_H

#include "violet/component/Component.h"
#include "violet/math/Polygon.h"
#include "violet/task/Thread.h"

namespace Violet
{
	class AlterContext;
	class ComponentDeserializer;
	class Serializer;

	class VIOLET_API MouseInputComponent : public ComponentBase<MouseInputComponent>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		MouseInputComponent(EntityId entityId, ComponentDeserializer & deserializer);
		MouseInputComponent(EntityId entityId, Polygon && mesh);
		MouseInputComponent(MouseInputComponent && other);

	public:

		Polygon m_mesh;
	};

	VIOLET_API Serializer & operator<<(Serializer & serializer, const MouseInputComponent & component);
	VIOLET_API ComponentDeserializer & operator>>(ComponentDeserializer & deserializer, MouseInputComponent & component);
}

#endif