#ifndef VIOLET_MouseInputComponent_H
#define VIOLET_MouseInputComponent_H

#include "violet/component/Component.h"
#include "violet/math/Polygon.h"

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

	public:

		MouseInputComponent(Entity & owner, Deserializer & deserializer);
		MouseInputComponent(Entity & owner, Polygon && mesh);
		MouseInputComponent(MouseInputComponent && other);
		//MouseInputComponent & operator=(MouseInputComponent && other);

	public:

		Polygon m_mesh;
	};

	Serializer & operator<<(Serializer & serializer, const MouseInputComponent & component);
	Deserializer & operator>>(Deserializer & deserializer, MouseInputComponent & component);
}

#endif