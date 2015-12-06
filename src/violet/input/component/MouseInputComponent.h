#ifndef VIOLET_MouseInputComponent_H
#define VIOLET_MouseInputComponent_H

#include "violet/component/Component.h"
#include "violet/math/Polygon.h"
#include "violet/task/Thread.h"

#include <functional>

namespace Violet
{
	class AlterContext;
	class Deserializer;
	class Serializer;

	class VIOLET_API MouseInputComponent : public ComponentBase<MouseInputComponent>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		MouseInputComponent(Handle entityId, Deserializer & deserializer);
		MouseInputComponent(Handle entityId, Polygon && mesh);
		MouseInputComponent(MouseInputComponent && other);

	public:

		Polygon m_mesh;
	};

	VIOLET_API Serializer & operator<<(Serializer & serializer, const MouseInputComponent & component);
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, MouseInputComponent & component);
}

#endif