// ============================================================================

#include "violet/script/cpp/CppScript.h"

#include "violet/log/Log.h"
#include "violet/script/ScriptFactory.h"
#include "violet/script/cpp/SharedLibrary.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"
#include "violet/utility/FileUtilities.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/StringUtilities.h"

#include <assert.h>
#include <set>

using namespace Violet;

// ============================================================================

namespace CppScriptNamespace
{
	typedef void(*Initializer)(CppScript & script, std::unique_ptr<CppScript::Instance> & instance);

	const char * const ms_swapSuffix = ".swp";

	std::unique_ptr<Script> create(const char * fileName);
}

using namespace CppScriptNamespace;

// ============================================================================

CppScript::Instance::Instance(CppScript & script)
	: m_script(script)
{
}

// ----------------------------------------------------------------------------

CppScript::Instance::~Instance()
{
}

// ============================================================================

void CppScript::install()
{
	ScriptFactory::assign(SharedLibrary::getSuffix(), create);
}

// ============================================================================

CppScript::CppScript(const char * const fileName) :
	m_fileName(fileName),
	m_lib(),
	m_instance()
{
	load();
}

// ----------------------------------------------------------------------------

CppScript::CppScript(CppScript && other) :
	m_fileName(std::move(other.m_fileName)),
	m_lib(std::move(other.m_lib)),
	m_instance(std::move(other.m_instance))
{
	other.m_lib.reset();
	other.m_instance = nullptr;
}

// ----------------------------------------------------------------------------

CppScript::~CppScript()
{
	unload();
}

// ----------------------------------------------------------------------------

const std::string & CppScript::getFileName() const
{
	return m_fileName;
}

// ----------------------------------------------------------------------------

bool CppScript::isValid() const
{
	return m_lib != nullptr;
}

// ----------------------------------------------------------------------------

void CppScript::reload()
{
	const std::string swapFileName = m_fileName + ms_swapSuffix;
	if (FileUtilities::exists(swapFileName.c_str()))
	{
		if (isValid())
			unload();

		std::remove(m_fileName.c_str());
		if (FileUtilities::copy(swapFileName.c_str(), m_fileName.c_str()))
			load();
		else
			Log::log(FormattedString<256>().sprintf("Could not copy file '%s%s' to '%s'", m_fileName.c_str(), ms_swapSuffix, m_fileName.c_str()));
	}
}

// ============================================================================

void * CppScript::getMethodPtr(const char * const name) const
{
	void * methodPtr = nullptr;
	if (isValid())
	{
		methodPtr = m_lib->getMethodPtr(name);
		if (methodPtr == nullptr)
			warn(name, "");
	}
	return methodPtr;
}

// ----------------------------------------------------------------------------

void CppScript::load()
{
	m_lib = SharedLibrary::load(m_fileName.c_str());
	if (!isValid())
		Log::log(FormattedString<128>().sprintf("Error loading script: '%s'", m_fileName.c_str()));
	else
	{
		auto init = (Initializer)getMethodPtr("init");
		if (init != nullptr)
		{
			init(*this, m_instance);
			assert(m_instance != nullptr);
		}
	}
}

// ----------------------------------------------------------------------------

void CppScript::unload()
{
	m_instance.reset();
	m_lib.reset();
}

// ============================================================================

std::unique_ptr<Script> CppScriptNamespace::create(const char * const fileName)
{
	return std::make_unique<CppScript>(fileName);
}

// ============================================================================
