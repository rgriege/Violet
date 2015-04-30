#ifndef UPDATE_COMPONENT_H
#define UPDATE_COMPONENT_H

#include "engine/component/Component.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API UpdateComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		UpdateComponent(const Entity & entity, Deserializer & deserializer);
		UpdateComponent(UpdateComponent && other);
		UpdateComponent & operator=(UpdateComponent && other);
	};

	Serializer & operator<<(Serializer & serializer, const UpdateComponent & component);
}

#endif