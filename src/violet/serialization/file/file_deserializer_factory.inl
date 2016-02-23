// ============================================================================

template <typename StreamDeserializer>
std::unique_ptr<vlt::Deserializer> vlt::File_Deserializer_Factory::produce(std::filebuf && file)
{
	return std::unique_ptr<Deserializer>(new File_Deserializer<StreamDeserializer>(std::move(file)));
}

// ----------------------------------------------------------------------------

template <typename T>
void vlt::File_Deserializer_Factory::assign(const char * extension)
{
	m_factory.assign(extension, &produce<T>);
}

// ============================================================================
