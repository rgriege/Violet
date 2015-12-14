#ifndef VIOLET_SerializationUtilities_H
#define VIOLET_SerializationUtilities_H

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"
#include "violet/structures/Vector.h"

namespace Violet
{
	class SerializationUtilities
	{
	public:

		template <typename T>
		static Vector<T> deserializeElements(Deserializer & deserializer);

		template <typename T>
		static void serializeElements(Serializer & serializer, const Vector<T> & elements);
	};
}

#include "violet/serialization/SerializationUtilities.inl"

#endif