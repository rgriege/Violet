#ifndef VIOLET_SerializationUtilities_H
#define VIOLET_SerializationUtilities_H

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"

#include <vector>

namespace Violet
{
	class SerializationUtilities
	{
	public:

		template <typename T>
		static std::vector<T> deserializeElements(Deserializer & deserializer);

		template <typename T>
		static void serializeElements(Serializer & serializer, const std::vector<T> & elements);
	};
}

#include "engine/serialization/SerializationUtilities.inl"

#endif