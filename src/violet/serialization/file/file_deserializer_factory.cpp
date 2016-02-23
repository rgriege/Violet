// ============================================================================

#include "violet/serialization/file/file_deserializer_factory.h"

using namespace vlt;

// ============================================================================

File_Deserializer_Factory & File_Deserializer_Factory::instance()
{
	static File_Deserializer_Factory ms_instance;
	return ms_instance;
}

// ============================================================================

std::unique_ptr<Deserializer> File_Deserializer_Factory::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::in))
		return nullptr;
	
	return m_factory.create(String_Utilities::rightOfLast(filename, '.'), std::move(fb));
}

// ----------------------------------------------------------------------------

void File_Deserializer_Factory::remove(const char * extension)
{
	m_factory.remove(extension);
}

// ============================================================================

File_Deserializer_Factory::File_Deserializer_Factory() :
	m_factory()
{
}

// ============================================================================
