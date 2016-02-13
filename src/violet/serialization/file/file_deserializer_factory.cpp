// ============================================================================

#include "violet/serialization/file/file_deserializer_factory.h"

using namespace vlt;

// ============================================================================

file_deserializer_factory & file_deserializer_factory::instance()
{
	static file_deserializer_factory ms_instance;
	return ms_instance;
}

// ============================================================================

std::unique_ptr<deserializer> file_deserializer_factory::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::in))
		return nullptr;
	
	return m_factory.create(string_utilities::rightOfLast(filename, '.'), std::move(fb));
}

// ----------------------------------------------------------------------------

void file_deserializer_factory::remove(const char * extension)
{
	m_factory.remove(extension);
}

// ============================================================================

file_deserializer_factory::file_deserializer_factory() :
	m_factory()
{
}

// ============================================================================
