// ============================================================================

template <typename T>
std::vector<T> Violet::SerializationUtilities::deserializeElements(Deserializer & deserializer)
{
	uint32 n = deserializer.getUint("n");
	std::vector<Vec2f> elements;
	elements.reserve(n);
	for (uint32 i = 0; i < n; ++i)
		elements.emplace_back(deserializer);
	return elements;
}

// ----------------------------------------------------------------------------

template <typename T>
void Violet::SerializationUtilities::serializeElements(Serializer & serializer, const std::vector<T> & elements)
{
	serializer.writeUint("n", elements.size());
	for (auto const & element : elements)
		serializer << element;
}

// ============================================================================
