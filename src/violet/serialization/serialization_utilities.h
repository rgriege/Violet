#ifndef VIOLET_SERIALIZATION_UTILITIES_H
#define VIOLET_SERIALIZATION_UTILITIES_H

#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"
#include "violet/structures/vector.h"

namespace vlt
{
	template <typename T>
	static Vector<T> deserialize_vector(Deserializer & d);

	template <typename T>
	static void serialize_vector(Serializer & s, const Vector<T> & elements);
}

#include "violet/serialization/serialization_utilities.inl"

#endif
