// ============================================================================

#include <string>

// ============================================================================

namespace vlt
{
	namespace detail
	{
		template <typename T>
		struct VectorDeserializer
		{
			static Vector<T> run(Deserializer & d, const u32 elementCount)
			{
				Vector<T> elements;
				elements.reserve(elementCount);
				for (u32 i = 0; i < elementCount; ++i)
					elements.emplace_back(d);
				return elements;
			}
		};

		template <>
		struct VectorDeserializer<std::string>
		{
			static Vector<std::string> run(Deserializer & d, const u32 elementCount)
			{
				Vector<std::string> elements;
				elements.reserve(elementCount);
				for (u32 i = 0; i < elementCount; ++i)
					elements.emplace_back(d.get_string("str"));
				return elements;
			}
		};
	}
}

// ============================================================================

template <typename T>
vlt::Vector<T> vlt::deserialize_vector(vlt::Deserializer & d)
{
	return detail::VectorDeserializer<T>::run(d, d.get_u32("n"));
}

// ----------------------------------------------------------------------------

template <typename T>
void vlt::serialize_vector(vlt::Serializer & s, const vlt::Vector<T> & elements)
{
	s.write_u32("n", elements.size());
	for (auto const & element : elements)
		s << element;
}

// ============================================================================
