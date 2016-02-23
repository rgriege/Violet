// ============================================================================

#include <Windows.h>

#include "violet/script/cpp/shared_library.h"
#include "violet/utility/string_utilities.h"

using namespace vlt;

// ============================================================================

struct Shared_Library::Implementation
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

std::shared_ptr<Shared_Library> Shared_Library::load(const char * const filename)
{
	HMODULE handle = LoadLibrary(filename);
	return handle != nullptr ? std::shared_ptr<Shared_Library>(new Shared_Library(make_unique_val<Shared_Library::Implementation>(handle))) : nullptr;
}

// ----------------------------------------------------------------------------

const char * Shared_Library::get_suffix()
{
	return "dll";
}

// ============================================================================

Shared_Library::~Shared_Library()
{
	FreeLibrary(m_impl->m_handle);
}

// ----------------------------------------------------------------------------

std::string Shared_Library::get_filename() const
{
	std::string filename;
	{
		static const u32 bufferSize = 128;
		char buffer[bufferSize];
		GetModuleFileName(m_impl->m_handle, buffer, bufferSize);
		filename = buffer;
	}

	return String_Utilities::rightOfLast(filename, '\\');
}

// ----------------------------------------------------------------------------

void * Shared_Library::get_method_ptr(const char * const name)
{
	return GetProcAddress(m_impl->m_handle, name);
}

// ============================================================================

Shared_Library::Shared_Library(unique_val<Implementation> && impl) :
	m_impl(std::move(impl))
{
}

// ============================================================================

