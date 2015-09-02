// ============================================================================

template <typename StreamDeserializer>
std::unique_ptr<Violet::Deserializer> Violet::FileDeserializerFactory::produce(std::filebuf && file)
{
	return std::unique_ptr<Deserializer>(new FileDeserializer<StreamDeserializer>(std::move(file)));
}

// ----------------------------------------------------------------------------

template <typename T>
void Violet::FileDeserializerFactory::assign(const char * extension)
{
	m_factory.assign(extension, &produce<T>);
}

// ============================================================================
