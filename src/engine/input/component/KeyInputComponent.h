#ifndef VIOLET_KeyInputComponent_H
#define VIOLET_KeyInputComponent_H

#include "engine/component/Component.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API KeyInputComponent : public ComponentBase<KeyInputComponent>
	{
	public:

		static Tag getTag();

	public:

		KeyInputComponent(const Entity & owner, Deserializer & deserializer);
		KeyInputComponent(KeyInputComponent && other);
		//KeyInputComponent & operator=(KeyInputComponent && other);
	};

	Serializer & operator<<(Serializer & serializer, const KeyInputComponent & component);
	Deserializer & operator>>(Deserializer & deserializer, KeyInputComponent & component);
}

#endif