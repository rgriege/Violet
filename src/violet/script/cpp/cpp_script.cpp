// ============================================================================

#include <assert.h>
#include <set>

#include "violet/log/log.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/script/script_factory.h"
#include "violet/script/cpp/shared_library.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"
#include "violet/utility/file_utilities.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/string_utilities.h"

using namespace vlt;

// ============================================================================

namespace Cpp_Script_Namespace
{
	typedef void(*Initializer)(Cpp_Script & Script, std::unique_ptr<Cpp_Script::Instance> & instance);

	const char * const ms_swapSuffix = ".swp";

	std::unique_ptr<Script> create(const char * fileName);
}

using namespace Cpp_Script_Namespace;

// ============================================================================

Cpp_Script::Instance::Instance(Cpp_Script & _script)
	: script(_script)
{
}

// ----------------------------------------------------------------------------

Cpp_Script::Instance::~Instance()
{
}

// ============================================================================

void Cpp_Script::install()
{
	Script_Factory::assign(Shared_Library::get_suffix(), create);
}

// ============================================================================

Cpp_Script::Cpp_Script(const char * const fileName) :
	m_fileName(fileName),
	m_lib(),
	m_instance()
{
	load();
}

// ----------------------------------------------------------------------------

Cpp_Script::Cpp_Script(Cpp_Script && other) :
	m_fileName(std::move(other.m_fileName)),
	m_lib(std::move(other.m_lib)),
	m_instance(std::move(other.m_instance))
{
	other.m_lib.reset();
	other.m_instance = nullptr;
}

// ----------------------------------------------------------------------------

Cpp_Script::~Cpp_Script()
{
	unload();
}

// ----------------------------------------------------------------------------

const std::string & Cpp_Script::get_filename() const
{
	return m_fileName;
}

// ----------------------------------------------------------------------------

bool Cpp_Script::is_valid() const
{
	return m_lib != nullptr;
}

// ----------------------------------------------------------------------------

void Cpp_Script::reload()
{
	const std::string swapFileName = m_fileName + ms_swapSuffix;
	if (File_Utilities::exists(swapFileName.c_str()))
	{
		if (is_valid())
			unload();

		std::remove(m_fileName.c_str());
		if (File_Utilities::copy(swapFileName.c_str(), m_fileName.c_str()))
			load();
		else
			log(Formatted_String<256>().sprintf("Could not copy file '%s%s' to '%s'", m_fileName.c_str(), ms_swapSuffix, m_fileName.c_str()));
	}
}

// ============================================================================

void * Cpp_Script::get_method_ptr(const char * const name) const
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

void Cpp_Script::load()
{
	m_lib = Shared_Library::load(m_fileName.c_str());
	if (!is_valid())
		log(Formatted_String<128>().sprintf("Error loading Script: '%s'", m_fileName.c_str()));
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

void Cpp_Script::unload()
{
	m_instance.reset();
	m_lib.reset();
}

// ============================================================================

std::unique_ptr<Script> Cpp_Script_Namespace::create(const char * const fileName)
{
	return std::make_unique<Cpp_Script>(fileName);
}

// ============================================================================
