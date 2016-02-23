// ============================================================================

#include "violet/serialization/file/file_serializer_factory.h"

using namespace vlt;

// ============================================================================

File_Serializer_Factory & File_Serializer_Factory::instance()
{
	static File_Serializer_Factory ms_instance;
	return ms_instance;
}

// ----------------------------------------------------------------------------

File_Serializer_Factory::File_Serializer_Factory() :
	m_factory()
{
}

// ----------------------------------------------------------------------------

std::unique_ptr<Serializer> File_Serializer_Factory::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::out))
		return nullptr;

	return m_factory.create(String_Utilities::rightOfLast(filename, '.'), std::move(fb));
}

// ----------------------------------------------------------------------------

void File_Serializer_Factory::remove(const char * extension)
{
	m_factory.remove(extension);
}

// ============================================================================
