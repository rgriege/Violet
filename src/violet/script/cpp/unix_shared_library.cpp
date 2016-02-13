// ============================================================================

#include "violet/script/cpp/shared_library.h"

#include <cstdio>
#include <dlfcn.h>

using namespace vlt;

// ============================================================================

class shared_library::implementation
{
public:

	implementation(void * handle, std::string filename) :
		m_handle(handle),
		m_filename(std::move(filename))
	{
	}

public:

	void * m_handle;
	std::string m_filename;
};

// ============================================================================

std::shared_ptr<shared_library> shared_library::load(const char * const filename)
{
	void * handle = dlopen(filename, RTLD_LAZY);
	return handle != nullptr ? std::shared_ptr<shared_library>(new shared_library(make_unique_val<shared_library::implementation>(handle, filename))) : nullptr;
}

// ----------------------------------------------------------------------------

const char * shared_library::get_suffix()
{
	return "so";
}

// ============================================================================

shared_library::~shared_library()
{
	int const result = dlclose(m_impl->m_handle);
	if (result != 0)
	{
		const char * const error = dlerror();
		printf("Error closing shared lib %s: %s (%d)\n", m_impl->m_filename.c_str(), error, result);
	}
}

// ----------------------------------------------------------------------------

std::string shared_library::get_filename() const
{
	return m_impl->m_filename;
}

// ----------------------------------------------------------------------------

void * shared_library::get_method_ptr(const char * const name)
{
	return dlsym(m_impl->m_handle, name);
}

// ============================================================================

shared_library::shared_library(unique_val<implementation> && impl) :
	m_impl(std::move(impl))
{
}

// ============================================================================

