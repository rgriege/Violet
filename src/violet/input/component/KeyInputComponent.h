#ifndef VIOLET_KeyInputComponent_H
#define VIOLET_KeyInputComponent_H

#include "violet/component/Component.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API KeyInputComponent : public ComponentBase<KeyInputComponent>
	{
	public:

		static Tag getStaticTag();

	public:

		KeyInputComponent(Entity & owner);
		KeyInputComponent(Entity & owner, Deserializer & deserializer);
		KeyInputComponent(KeyInputComponent && other);
		//KeyInputComponent & operator=(KeyInputComponent && other);
	};

	VIOLET_API Serializer & operator<<(Serializer & serializer, const KeyInputComponent & component);
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, KeyInputComponent & component);
}

#endif