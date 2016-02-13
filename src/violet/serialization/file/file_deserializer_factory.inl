// ============================================================================

template <typename StreamDeserializer>
std::unique_ptr<vlt::deserializer> vlt::file_deserializer_factory::produce(std::filebuf && file)
{
	return std::unique_ptr<deserializer>(new file_deserializer<StreamDeserializer>(std::move(file)));
}

// ----------------------------------------------------------------------------

template <typename T>
void vlt::file_deserializer_factory::assign(const char * extension)
{
	m_factory.assign(extension, &produce<T>);
}

// ============================================================================
