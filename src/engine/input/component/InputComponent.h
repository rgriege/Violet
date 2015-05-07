#ifndef VIOLET_InputComponent_H
#define VIOLET_InputComponent_H

#include "engine/component/Component.h"
#include "engine/math/Polygon.h"

#include <functional>

namespace Violet
{
	class AlterContext;
	class Deserializer;
	class Serializer;

	class VIOLET_API InputComponent : public Component<InputComponent>
	{
	public:

		static Tag getTypeId();

	public:

		InputComponent(Entity entity, Deserializer & deserializer);
		InputComponent(InputComponent && other);
		InputComponent & operator=(InputComponent && other);

	public:

		Polygon m_mesh;
	};

	Serializer & operator<<(Serializer & serializer, const InputComponent & component);
	Deserializer & operator>>(Deserializer & deserializer, InputComponent & component);
}

#endif