// ============================================================================

#include "violet/script/cpp/shared_library.h"

#include "violet/utility/string_utilities.h"

#include <Windows.h>

using namespace vlt;

// ============================================================================

struct shared_library::implementation
{
public:

	implementation(HMODULE handle) :
		m_handle(handle)
	{
	}

public:

	HMODULE m_handle;
};

// ============================================================================

std::shared_ptr<shared_library> shared_library::load(const char * const filename)
{
	HMODULE handle = LoadLibrary(filename);
	return handle != nullptr ? std::shared_ptr<shared_library>(new shared_library(make_unique_val<shared_library::implementation>(handle))) : nullptr;
}

// ----------------------------------------------------------------------------

const char * shared_library::get_suffix()
{
	return "dll";
}

// ============================================================================

shared_library::~shared_library()
{
	FreeLibrary(m_impl->m_handle);
}

// ----------------------------------------------------------------------------

std::string shared_library::get_filename() const
{
	std::string filename;
	{
		static const u32 bufferSize = 128;
		char buffer[bufferSize];
		GetModuleFileName(m_impl->m_handle, buffer, bufferSize);
		filename = buffer;
	}

	return string_utilities::rightOfLast(filename, '\\');
}

// ----------------------------------------------------------------------------

void * shared_library::get_method_ptr(const char * const name)
{
	return GetProcAddress(m_impl->m_handle, name);
}

// ============================================================================

shared_library::shared_library(unique_val<implementation> && impl) :
	m_impl(std::move(impl))
{
}

// ============================================================================

