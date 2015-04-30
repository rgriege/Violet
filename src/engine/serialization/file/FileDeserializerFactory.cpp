#include "engine/serialization/file/FileDeserializerFactory.h"

using namespace Violet;

FileDeserializerFactory & FileDeserializerFactory::getInstance()
{
	static FileDeserializerFactory ms_instance;
	return ms_instance;
}

FileDeserializerFactory::FileDeserializerFactory() :
	m_factory()
{
}

std::unique_ptr<Deserializer> FileDeserializerFactory::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::in))
		return nullptr;

	return m_factory.create(StringUtilities::right(filename, '.'), std::move(fb));
}

void FileDeserializerFactory::remove(const char * extension)
{
	m_factory.remove(extension);
}