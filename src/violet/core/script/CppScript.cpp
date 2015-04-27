#include "violet/core/script/CppScript.h"

#include "violet/core/Defines.h"
#include "violet/core/script/ScriptFactory.h"
#include "violet/core/utility/FileUtilities.h"
#include "violet/core/utility/StringUtilities.h"

#include <iostream>
#include <set>

using namespace Violet;

namespace CppScriptNamespace
{
	typedef void (*Initializer)(CppScript::Allocator & allocator);
	
	const char * const ms_extension = "dll";
	const char * const ms_debugExtension = "pdb";
	const char * const ms_swapSuffix = ".swp";

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
	m_lib(),
	m_allocator()
{
	load(filename);
}

CppScript::~CppScript()
{
	unload();
}

std::string CppScript::getFilename() const
{
	return StringUtilities::lastRight(getFilenameWithPath(), '\\');
}

void CppScript::run(ProcedureBase & procedure)
{
	procedure.run(*this);
}

void CppScript::reload()
{
	if (m_lib != nullptr)
	{
		const std::string filename = getFilename();

		unload();
		int retCode = std::remove(filename.c_str());
		if (retCode == 0)
		{
			if (FileUtilities::copy((filename + ms_swapSuffix).c_str(), filename.c_str()))
			{
#ifdef _DEBUG
				std::string pdbFilename = filename;
				StringUtilities::replace(pdbFilename, ms_extension, ms_debugExtension);
				retCode = std::remove(pdbFilename.c_str());
				if (retCode == 0)
				{
					if (!FileUtilities::copy((pdbFilename + ms_swapSuffix).c_str(), pdbFilename.c_str()))
						std::cout << "Could not copy file '" << pdbFilename << ms_swapSuffix << "' to '" << pdbFilename << "'" << std::endl;
				}
				else
					std::cout << "Could not remove file: " << pdbFilename << std::endl;
#endif
				load(filename.c_str());
			}
			else
				std::cout << "Could not copy file '" << filename << ms_swapSuffix << "' to '" << filename << "'" << std::endl;
		}
		else
			std::cout << "Could not remove file: " << filename << std::endl;
	}
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

std::string CppScript::getFilenameWithPath() const
{
	std::string filename;
	{
		static const uint32 bufferSize = 64;
		char buffer[bufferSize];
		GetModuleFileName(m_lib, buffer, bufferSize);
		filename = buffer;
	}

	return filename;
}

void CppScript::load(const char * filename)
{
	m_lib = LoadLibrary(filename);
	if (m_lib == nullptr)
		std::cout << "Error loading script: " << filename << std::endl;
	else
	{
		auto init = (Initializer)getMethodPtr("init");
		if (init != nullptr)
			init(m_allocator);
	}
}

void CppScript::unload()
{
	FreeLibrary(m_lib);
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