// ============================================================================

namespace Violet
{
	namespace detail
	{
		template <typename T>
		struct VectorDeserializer
		{
			static Vector<T> run(Deserializer & deserializer, const uint32 elementCount)
			{
				Vector<T> elements;
				elements.reserve(elementCount);
				for (uint32 i = 0; i < elementCount; ++i)
					elements.emplace_back(deserializer);
				return elements;
			}
		};

		template <>
		struct VectorDeserializer<std::string>
		{
			static Vector<std::string> run(Deserializer & deserializer, const uint32 elementCount)
			{
				Vector<std::string> elements;
				elements.reserve(elementCount);
				for (uint32 i = 0; i < elementCount; ++i)
					elements.emplace_back(deserializer.getString("str"));
				return elements;
			}
		};
	}
}

// ============================================================================

template <typename T>
Violet::Vector<T> Violet::SerializationUtilities::deserializeElements(Deserializer & deserializer)
{
	return detail::VectorDeserializer<T>::run(deserializer, deserializer.getUint("n"));
}

// ----------------------------------------------------------------------------

template <typename T>
void Violet::SerializationUtilities::serializeElements(Serializer & serializer, const Vector<T> & elements)
{
	serializer.writeUint("n", elements.size());
	for (auto const & element : elements)
		serializer << element;
}

// ============================================================================
