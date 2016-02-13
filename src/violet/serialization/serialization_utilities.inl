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
			static vector<T> run(deserializer & d, const u32 elementCount)
			{
				vector<T> elements;
				elements.reserve(elementCount);
				for (u32 i = 0; i < elementCount; ++i)
					elements.emplace_back(d);
				return elements;
			}
		};

		template <>
		struct VectorDeserializer<std::string>
		{
			static vector<std::string> run(deserializer & d, const u32 elementCount)
			{
				vector<std::string> elements;
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
vlt::vector<T> vlt::deserialize_vector(vlt::deserializer & d)
{
	return detail::VectorDeserializer<T>::run(d, d.get_u32("n"));
}

// ----------------------------------------------------------------------------

template <typename T>
void vlt::serialize_vector(vlt::serializer & s, const vlt::vector<T> & elements)
{
	s.write_u32("n", elements.size());
	for (auto const & element : elements)
		s << element;
}

// ============================================================================
