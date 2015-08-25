// ============================================================================

#include "engine/script/cpp/SharedLibrary.h"

#include "engine/utility/StringUtilities.h"

#include <Windows.h>

using namespace Violet;

// ============================================================================

class SharedLibrary::Implementation
{
public:

	Implementation(HMODULE handle) :
		m_handle(handle)
	{
	}

public:

    HMODULE m_handle;
};

// ============================================================================

std::shared_ptr<SharedLibrary> SharedLibrary::load(const char * const filename)
{
    HMODULE handle = LoadLibrary(filename);
    return handle != nullptr ? std::shared_ptr<SharedLibrary>(new SharedLibrary(make_unique_val<SharedLibrary::Implementation>(handle))) : nullptr;
}

// ============================================================================

SharedLibrary::~SharedLibrary()
{
    FreeLibrary(m_impl->m_handle);
}

// ----------------------------------------------------------------------------

std::string SharedLibrary::getFilename() const
{
	std::string filename;
	{
		static const uint32 bufferSize = 128;
		char buffer[bufferSize];
		GetModuleFileName(m_impl->m_handle, buffer, bufferSize);
		filename = buffer;
	}

	return StringUtilities::lastRight(filename, '\\');
}

// ----------------------------------------------------------------------------

void * SharedLibrary::getMethodPtr(const char * const name)
{
	return GetProcAddress(m_impl->m_handle, name);
}

// ============================================================================

SharedLibrary::SharedLibrary(unique_val<Implementation> && impl) :
    m_impl(std::move(impl))
{
}

// ============================================================================

