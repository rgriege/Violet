#include "violet/serialization/file/FileSerializerFactory.h"

using namespace Violet;

FileSerializerFactory & FileSerializerFactory::getInstance()
{
	static FileSerializerFactory ms_instance;
	return ms_instance;
}

FileSerializerFactory::FileSerializerFactory() :
	m_factory()
{
}

std::unique_ptr<Serializer> FileSerializerFactory::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::out))
		return nullptr;

	return m_factory.create(StringUtilities::rightOfLast(filename, '.'), std::move(fb));
}

void FileSerializerFactory::remove(const char * extension)
{
	m_factory.remove(extension);
}