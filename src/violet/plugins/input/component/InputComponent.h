#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "violet/core/component/Component.h"
#include "violet/core/math/Polygon.h"

#include <functional>

#ifdef WIN32
#ifdef VIOLETINPUT_EXPORT
#define VIOLET_INPUT_API __declspec(dllexport)
#else
#define VIOLET_INPUT_API __declspec(dllimport)
#endif
#else
#define VIOLET_API
#endif

namespace Violet
{
	class AlterContext;
	class Deserializer;
	class Serializer;

	class VIOLET_INPUT_API InputComponent : public Component
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