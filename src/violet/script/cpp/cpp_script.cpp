// ============================================================================

#include "violet/script/cpp/cpp_script.h"

#include "violet/log/Log.h"
#include "violet/script/script_factory.h"
#include "violet/script/cpp/shared_library.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"
#include "violet/utility/file_utilities.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/string_utilities.h"

#include <assert.h>
#include <set>

using namespace vlt;

// ============================================================================

namespace cpp_scriptNamespace
{
	typedef void(*Initializer)(cpp_script & script, std::unique_ptr<cpp_script::instance> & instance);

	const char * const ms_swapSuffix = ".swp";

	std::unique_ptr<script> create(const char * fileName);
}

using namespace cpp_scriptNamespace;

// ============================================================================

cpp_script::instance::instance(cpp_script & script)
	: script(script)
{
}

// ----------------------------------------------------------------------------

cpp_script::instance::~instance()
{
}

// ============================================================================

void cpp_script::install()
{
	script_factory::assign(shared_library::get_suffix(), create);
}

// ============================================================================

cpp_script::cpp_script(const char * const fileName) :
	m_fileName(fileName),
	m_lib(),
	m_instance()
{
	load();
}

// ----------------------------------------------------------------------------

cpp_script::cpp_script(cpp_script && other) :
	m_fileName(std::move(other.m_fileName)),
	m_lib(std::move(other.m_lib)),
	m_instance(std::move(other.m_instance))
{
	other.m_lib.reset();
	other.m_instance = nullptr;
}

// ----------------------------------------------------------------------------

cpp_script::~cpp_script()
{
	unload();
}

// ----------------------------------------------------------------------------

const std::string & cpp_script::get_filename() const
{
	return m_fileName;
}

// ----------------------------------------------------------------------------

bool cpp_script::is_valid() const
{
	return m_lib != nullptr;
}

// ----------------------------------------------------------------------------

void cpp_script::reload()
{
	const std::string swapFileName = m_fileName + ms_swapSuffix;
	if (file_utilities::exists(swapFileName.c_str()))
	{
		if (is_valid())
			unload();

		std::remove(m_fileName.c_str());
		if (file_utilities::copy(swapFileName.c_str(), m_fileName.c_str()))
			load();
		else
			log(formatted_string<256>().sprintf("Could not copy file '%s%s' to '%s'", m_fileName.c_str(), ms_swapSuffix, m_fileName.c_str()));
	}
}

// ============================================================================

void * cpp_script::get_method_ptr(const char * const name) const
{
	void * methodPtr = nullptr;
	if (is_valid())
	{
		methodPtr = m_lib->get_method_ptr(name);
		if (methodPtr == nullptr)
			warn(name, "");
	}
	return methodPtr;
}

// ----------------------------------------------------------------------------

void cpp_script::load()
{
	m_lib = shared_library::load(m_fileName.c_str());
	if (!is_valid())
		log(formatted_string<128>().sprintf("Error loading script: '%s'", m_fileName.c_str()));
	else
	{
		auto init = (Initializer)get_method_ptr("init");
		if (init != nullptr)
		{
			init(*this, m_instance);
			assert(m_instance != nullptr);
		}
	}
}

// ----------------------------------------------------------------------------

void cpp_script::unload()
{
	m_instance.reset();
	m_lib.reset();
}

// ============================================================================

std::unique_ptr<script> cpp_scriptNamespace::create(const char * const fileName)
{
	return std::make_unique<cpp_script>(fileName);
}

// ============================================================================
