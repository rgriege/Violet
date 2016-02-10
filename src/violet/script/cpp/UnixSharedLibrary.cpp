// ============================================================================

#include "violet/script/cpp/SharedLibrary.h"

#include <cstdio>
#include <dlfcn.h>

using namespace Violet;

// ============================================================================

class SharedLibrary::Implementation
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

std::shared_ptr<SharedLibrary> SharedLibrary::load(const char * const filename)
{
	void * handle = dlopen(filename, RTLD_LAZY);
	return handle != nullptr ? std::shared_ptr<SharedLibrary>(new SharedLibrary(make_unique_val<SharedLibrary::Implementation>(handle, filename))) : nullptr;
}

// ----------------------------------------------------------------------------

const char * SharedLibrary::getSuffix()
{
	return "so";
}

// ============================================================================

SharedLibrary::~SharedLibrary()
{
	int const result = dlclose(m_impl->m_handle);
	if (result != 0)
	{
		const char * const error = dlerror();
		printf("Error closing shared lib %s: %s (%d)\n", m_impl->m_filename.c_str(), error, result);
	}
}

// ----------------------------------------------------------------------------

std::string SharedLibrary::getFilename() const
{
	return m_impl->m_filename;
}

// ----------------------------------------------------------------------------

void * SharedLibrary::getMethodPtr(const char * const name)
{
	return dlsym(m_impl->m_handle, name);
}

// ============================================================================

SharedLibrary::SharedLibrary(unique_val<Implementation> && impl) :
	m_impl(std::move(impl))
{
}

// ============================================================================

