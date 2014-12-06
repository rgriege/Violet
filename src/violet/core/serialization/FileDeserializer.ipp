using namespace Violet;

#include <fstream>

template <class StreamDeserializer>
std::unique_ptr<FileDeserializer<StreamDeserializer>> FileDeserializer<StreamDeserializer>::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::in))
		return nullptr;

	return std::unique_ptr<FileDeserializer<StreamDeserializer>>(new FileDeserializer<StreamDeserializer>(std::move(fb)));
}

template <class StreamDeserializer>
FileDeserializer<StreamDeserializer>::~FileDeserializer()
{
	m_file.close();
}

template <class StreamDeserializer>
FileDeserializer<StreamDeserializer>::operator bool() const
{
	return m_deserializer;
}

template <class StreamDeserializer>
std::unique_ptr<Deserializer> FileDeserializer<StreamDeserializer>::enterSegment(const char * label)
{
	return m_deserializer.enterSegment(label);
}

template <class StreamDeserializer>
const char * FileDeserializer<StreamDeserializer>::nextLabel() const
{
	return m_deserializer.nextLabel();
}

template <class StreamDeserializer>
bool FileDeserializer<StreamDeserializer>::getBoolean(const char * label)
{
	return m_deserializer.getBoolean(label);
}

template <class StreamDeserializer>
uint32 FileDeserializer<StreamDeserializer>::getUint(const char * label)
{
	return m_deserializer.getUint(label);
}

template <class StreamDeserializer>
int32 FileDeserializer<StreamDeserializer>::getInt(const char * label)
{
	return m_deserializer.getInt(label);
}

template <class StreamDeserializer>
float FileDeserializer<StreamDeserializer>::getFloat(const char * label)
{
	return m_deserializer.getFloat(label);
}

template <class StreamDeserializer>
double FileDeserializer<StreamDeserializer>::getDouble(const char * label)
{
	return m_deserializer.getDouble(label);
}

template <class StreamDeserializer>
const char * FileDeserializer<StreamDeserializer>::getString(const char * label)
{
	return m_deserializer.getString(label);
}

template <class StreamDeserializer>
FileDeserializer<StreamDeserializer>::FileDeserializer(std::filebuf && file) :
	m_file(std::move(file)),
	m_stream(&m_file),
	m_deserializer(m_stream)
{
}