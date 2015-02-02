#include "violet/core/script/CppScript.h"

#include "violet/core/Defines.h"
#include "violet/core/script/ScriptFactory.h"

#include <iostream>
#include <set>

using namespace Violet;

namespace CppScriptNamespace
{
	typedef void (*Initializer)(CppScript::Allocator & allocator);
	
	const char * const ms_extension = "dll";

	std::unique_ptr<Script> create(const char * filename);
	void warnMissing(const char * procedureName, HMODULE lib);
}

using namespace CppScriptNamespace;

CppScript::Allocator::Allocator() :
	m_memory(nullptr)
{
}

CppScript::Allocator::~Allocator()
{
	if (m_memory != nullptr)
		free(m_memory);
}

void * CppScript::Allocator::allocate(size_t size)
{
	if (m_memory != nullptr)
		free(m_memory);

	m_memory = malloc(size);
	return m_memory;
}

void * CppScript::Allocator::fetch() const
{
	return m_memory;
}

void CppScript::install()
{
	ScriptFactory::getInstance().assign(ms_extension, &create);
}

CppScript::CppScript(const char * filename) :
	m_lib(LoadLibrary(filename)),
	m_allocator()
{
	if (m_lib == nullptr)
		std::cout << "Error loading script: " << filename << std::endl;
	else
	{
		auto init = (Initializer)GetProcAddress(m_lib, "init");
		if (init != nullptr)
			init(m_allocator);
		else
			warnMissing("init", m_lib);
	}
}

CppScript::~CppScript()
{
	FreeLibrary(m_lib);
}

void CppScript::run(ProcedureBase & procedure)
{
	procedure.run(*this);
}

void * CppScript::getMethodPtr(const char * name) const
{
	void * methodPtr = GetProcAddress(m_lib, name);
	if (methodPtr == nullptr)
		warnMissing(name, m_lib);
	return methodPtr;
}

void * CppScript::getMemoryPtr() const
{
	return m_allocator.fetch();
}

std::unique_ptr<Script> CppScriptNamespace::create(const char * filename)
{
	return std::unique_ptr<Script>(new CppScript(filename));
}

void CppScriptNamespace::warnMissing(const char * procedureName, HMODULE lib)
{
	static std::set<std::string> ms_missingProcedures;

	static const uint32 bufferSize = 64;
	char filename[bufferSize];
	GetModuleFileName(lib, filename, bufferSize);
	std::string fileAndProcedure(filename);
	fileAndProcedure.append(procedureName);
	auto it = ms_missingProcedures.find(fileAndProcedure);
	if (it == ms_missingProcedures.end())
	{
		std::cout << "Error loading function " << procedureName << " in script " << filename << std::endl;
		ms_missingProcedures.insert(fileAndProcedure);
	}
}