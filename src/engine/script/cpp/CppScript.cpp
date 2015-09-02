// ============================================================================

#include "engine/script/cpp/CppScript.h"

#include "engine/script/ScriptFactory.h"
#include "engine/script/cpp/SharedLibrary.h"
#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"
#include "engine/utility/FileUtilities.h"
#include "engine/utility/StringUtilities.h"

#include <assert.h>
#include <iostream>
#include <set>

using namespace Violet;

// ============================================================================

namespace CppScriptNamespace
{
	typedef void(*Initializer)(CppScript & script, std::unique_ptr<CppScript::Memory> & memory);
	typedef void(*Cleaner)(CppScript & script, std::unique_ptr<CppScript::Memory> & memory);

	const char * const ms_swapSuffix = ".swp";

	std::unique_ptr<Script> create(const char * fileName);
}

using namespace CppScriptNamespace;

// ============================================================================

CppScript::Memory::~Memory()
{
}

// ============================================================================

void CppScript::install()
{
	ScriptFactory::assign("dll", create);
}

// ============================================================================

CppScript::CppScript(const char * const fileName) :
	m_fileName(fileName),
	m_lib(),
	m_memory()
{
	load();
}

// ----------------------------------------------------------------------------

CppScript::CppScript(CppScript && other) :
	m_fileName(std::move(other.m_fileName)),
	m_lib(std::move(other.m_lib)),
	m_memory(std::move(other.m_memory))
{
	other.m_lib.reset();
	other.m_memory = nullptr;
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
	if (isValid())
		unload();

	std::remove(m_fileName.c_str());
	if (FileUtilities::copy((m_fileName + ms_swapSuffix).c_str(), m_fileName.c_str()))
		load();
	else
		std::cout << "Could not copy file '" << m_fileName << ms_swapSuffix << "' to '" << m_fileName << "'" << std::endl;
}

// ============================================================================

void * CppScript::getMethodPtr(const char * const name) const
{
    void * methodPtr = m_lib->getMethodPtr(name);
	if (methodPtr == nullptr)
		warn(name, "");
	return methodPtr;
}

// ----------------------------------------------------------------------------

void CppScript::load()
{
	m_lib = SharedLibrary::load(m_fileName.c_str());
	if (!isValid())
		std::cout << "Error loading script: " << m_fileName << std::endl;
	else
	{
		auto init = (Initializer)getMethodPtr("init");
		if (init != nullptr)
			init(*this, m_memory);
	}
}

// ----------------------------------------------------------------------------

void CppScript::unload()
{
	auto clean = (Cleaner)getMethodPtr("clean");
	if (clean != nullptr)
		clean(*this, m_memory);
	m_memory.reset();
    m_lib.reset();
}

// ============================================================================

std::unique_ptr<Script> CppScriptNamespace::create(const char * const fileName)
{
	return std::make_unique<CppScript>(fileName);
}

// ============================================================================
