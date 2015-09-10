// ============================================================================

template <class StreamDeserializer>
Violet::FileDeserializer<StreamDeserializer>::FileDeserializer(std::filebuf && file) :
	m_file(static_cast<std::filebuf&&>(file)),
	m_stream(&m_file),
	m_deserializer(m_stream)
{
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
Violet::FileDeserializer<StreamDeserializer>::~FileDeserializer()
{
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
Violet::FileDeserializer<StreamDeserializer>::operator bool() const
{
	return m_deserializer;
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
std::unique_ptr<Violet::Deserializer> Violet::FileDeserializer<StreamDeserializer>::enterSegment(const char * label)
{
	return m_deserializer.enterSegment(label);
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
const char * Violet::FileDeserializer<StreamDeserializer>::nextLabel() const
{
	return m_deserializer.nextLabel();
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
bool Violet::FileDeserializer<StreamDeserializer>::getBoolean(const char * label)
{
	return m_deserializer.getBoolean(label);
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
uint32 Violet::FileDeserializer<StreamDeserializer>::getUint(const char * label)
{
	return m_deserializer.getUint(label);
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
int32 Violet::FileDeserializer<StreamDeserializer>::getInt(const char * label)
{
	return m_deserializer.getInt(label);
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
float Violet::FileDeserializer<StreamDeserializer>::getFloat(const char * label)
{
	return m_deserializer.getFloat(label);
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
double Violet::FileDeserializer<StreamDeserializer>::getDouble(const char * label)
{
	return m_deserializer.getDouble(label);
}

// ----------------------------------------------------------------------------

template <class StreamDeserializer>
const char * Violet::FileDeserializer<StreamDeserializer>::getString(const char * label)
{
	return m_deserializer.getString(label);
}

// ============================================================================
