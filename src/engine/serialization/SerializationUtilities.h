#ifndef SERIALIZATION_UTILITIES_H
#define SERIALIZATION_UTILITIES_H

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

	template <typename T>
	static std::vector<T> SerializationUtilities::deserializeElements(Deserializer & deserializer)
	{
		uint32 n = deserializer.getUint("n");
		std::vector<Vec2f> elements;
		elements.reserve(n);
		for (uint32 i = 0; i < n; ++i)
			elements.emplace_back(deserializer);
		return elements;
	}

	template <typename T>
	static void SerializationUtilities::serializeElements(Serializer & serializer, const std::vector<T> & elements)
	{
		serializer.writeUint("n", elements.size());
		for (auto const & element : elements)
			serializer << element;
	}
}

#endif