// ============================================================================

#include "violet/serialization/file/file_serializer_factory.h"

using namespace vlt;

// ============================================================================

file_serializer_factory & file_serializer_factory::instance()
{
	static file_serializer_factory ms_instance;
	return ms_instance;
}

// ----------------------------------------------------------------------------

file_serializer_factory::file_serializer_factory() :
	m_factory()
{
}

// ----------------------------------------------------------------------------

std::unique_ptr<serializer> file_serializer_factory::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::out))
		return nullptr;

	return m_factory.create(string_utilities::rightOfLast(filename, '.'), std::move(fb));
}

// ----------------------------------------------------------------------------

void file_serializer_factory::remove(const char * extension)
{
	m_factory.remove(extension);
}

// ============================================================================
