#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "engine/component/Component.h"
#include "engine/math/Polygon.h"

#include <functional>

namespace Violet
{
	class AlterContext;
	class Deserializer;
	class Serializer;

	class VIOLET_API InputComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		InputComponent(const Entity & entity, Deserializer & deserializer);
		InputComponent(InputComponent && other);
		InputComponent & operator=(InputComponent && other);

	public:

		Polygon m_mesh;
	};

	Serializer & operator<<(Serializer & serializer, const InputComponent & component);
}

#endif