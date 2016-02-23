// ============================================================================

#include <cstdio>
#include <dlfcn.h>

#include "violet/log/log.h"
#include "violet/script/cpp/shared_library.h"
#include "violet/utility/formatted_string.h"

using namespace vlt;

// ============================================================================

class Shared_Library::Implementation
{
public:

	Implementation(void * handle, std::string filename) :
		m_handle(handle),
		m_filename(std::move(filename))
	{
	}

public:

	void * m_handle;
	std::string m_filename;
};

// ============================================================================

std::shared_ptr<Shared_Library> Shared_Library::load(const char * const filename)
{
	void * handle = dlopen(filename, RTLD_LAZY);
	return handle != nullptr ? std::shared_ptr<Shared_Library>(new Shared_Library(make_unique_val<Shared_Library::Implementation>(handle, filename))) : nullptr;
}

// ----------------------------------------------------------------------------

const char * Shared_Library::get_suffix()
{
	return "so";
}

// ============================================================================

Shared_Library::~Shared_Library()
{
	int const result = dlclose(m_impl->m_handle);
	if (result != 0)
	{
		const char * const error = dlerror();
		log(Formatted_String<1024>().sprintf("Error closing shared lib %s: %s (%d)\n", m_impl->m_filename.c_str(), error, result));
	}
}

// ----------------------------------------------------------------------------

std::string Shared_Library::get_filename() const
{
	return m_impl->m_filename;
}

// ----------------------------------------------------------------------------

void * Shared_Library::get_method_ptr(const char * const name)
{
	return dlsym(m_impl->m_handle, name);
}

// ============================================================================

Shared_Library::Shared_Library(unique_val<Implementation> && impl) :
	m_impl(std::move(impl))
{
}

// ============================================================================

