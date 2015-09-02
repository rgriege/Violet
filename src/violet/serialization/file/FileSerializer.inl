using namespace Violet;

template <class StreamSerializer>
FileSerializer<StreamSerializer>::FileSerializer(std::filebuf && file) :
	m_file(std::move(file)),
	m_stream(&m_file),
	m_serializer(m_stream)
{
}

template <class StreamSerializer>
FileSerializer<StreamSerializer>::~FileSerializer()
{
}

template <class StreamSerializer>
unique_val<Serializer> FileSerializer<StreamSerializer>::createSegment(const char * label)
{
	return m_serializer.createSegment(label);
}

template <class StreamSerializer>
void FileSerializer<StreamSerializer>::writeBoolean(const char * label, bool const value)
{
	m_serializer.writeBoolean(label, value);
}

template <class StreamSerializer>
void FileSerializer<StreamSerializer>::writeUint(const char * label, uint32 const value)
{
	m_serializer.writeUint(label, value);
}

template <class StreamSerializer>
void FileSerializer<StreamSerializer>::writeInt(const char * label, int32 const value)
{
	m_serializer.writeInt(label, value);
}

template <class StreamSerializer>
void FileSerializer<StreamSerializer>::writeFloat(const char * label, float const value)
{
	m_serializer.writeFloat(label, value);
}

template <class StreamSerializer>
void FileSerializer<StreamSerializer>::writeDouble(const char * label, double const value)
{
	m_serializer.writeDouble(label, value);
}

template <class StreamSerializer>
void FileSerializer<StreamSerializer>::writeString(const char * label, const char * value)
{
	m_serializer.writeString(label, value);
}