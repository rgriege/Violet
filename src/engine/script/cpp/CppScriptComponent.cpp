// ============================================================================

#include "engine/script/cpp/CppScriptComponent.h"

#include "engine/script/cpp/SharedLibrary.h"
#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"
#include "engine/utility/FileUtilities.h"
#include "engine/utility/StringUtilities.h"

#include <iostream>
#include <set>

using namespace Violet;

// ============================================================================

namespace CppScriptComponentNamespace
{
	typedef void(*Initializer)(CppScriptComponent::Allocator & allocator, const Entity & entity);
	typedef void(*Cleaner)(const Entity & entity);

	const char * const ms_swapSuffix = ".swp";

	void warnMissing(const char * filename, const char * procedureName);
}

using namespace CppScriptComponentNamespace;

// ============================================================================

CppScriptComponent::Allocator::Allocator() :
	m_memory(nullptr)
{
}

// ----------------------------------------------------------------------------

CppScriptComponent::Allocator::Allocator(Allocator && other) :
	m_memory(other.m_memory)
{
	other.m_memory = nullptr;
}

// ----------------------------------------------------------------------------

CppScriptComponent::Allocator & CppScriptComponent::Allocator::operator=(Allocator && other)
{
	std::swap(m_memory, other.m_memory);
	return *this;
}

// ----------------------------------------------------------------------------

CppScriptComponent::Allocator::~Allocator()
{
	if (m_memory != nullptr)
		free(m_memory);
}

// ----------------------------------------------------------------------------

void * CppScriptComponent::Allocator::fetch() const
{
	return m_memory;
}

// ----------------------------------------------------------------------------

void * CppScriptComponent::Allocator::allocate(const size_t size)
{
	if (m_memory != nullptr)
		free(m_memory);

	m_memory = malloc(size);
	return m_memory;
}

// ============================================================================

Tag CppScriptComponent::getStaticTag()
{
	return Tag('c', 'p', 'p', 's');
}

// ============================================================================

CppScriptComponent::CppScriptComponent(const Entity & owner, Deserializer & deserializer) :
	ComponentBase<CppScriptComponent>(owner),
	m_lib(),
	m_allocator()
{
	load(deserializer.getString("file"));
}

// ----------------------------------------------------------------------------

CppScriptComponent::CppScriptComponent(CppScriptComponent && other) :
	ComponentBase<CppScriptComponent>(std::move(other)),
	m_lib(std::move(other.m_lib)),
	m_allocator(std::move(other.m_allocator))
{
	other.m_lib.reset();
}

// ----------------------------------------------------------------------------

CppScriptComponent::~CppScriptComponent()
{
	unload();
}

// ----------------------------------------------------------------------------

std::string CppScriptComponent::getFilename() const
{
    return m_lib != nullptr ? m_lib->getFilename() : "";
}

// ----------------------------------------------------------------------------

void CppScriptComponent::reload()
{
	if (m_lib != nullptr)
	{
		const std::string filename = getFilename();

		unload();
		int retCode = std::remove(filename.c_str());
		if (retCode == 0)
		{
			if (FileUtilities::copy((filename + ms_swapSuffix).c_str(), filename.c_str()))
				load(filename.c_str());
			else
				std::cout << "Could not copy file '" << filename << ms_swapSuffix << "' to '" << filename << "'" << std::endl;
		}
		else
			std::cout << "Could not remove file: " << filename << std::endl;
	}
}

// ============================================================================

void * CppScriptComponent::getMethodPtr(const char * const name) const
{
    void * methodPtr = m_lib->getMethodPtr(name);
	if (methodPtr == nullptr)
		warnMissing(getFilename().c_str(), name);
	return methodPtr;
}

// ----------------------------------------------------------------------------

void * CppScriptComponent::getMemoryPtr() const
{
	return m_allocator.fetch();
}

// ----------------------------------------------------------------------------

void CppScriptComponent::load(const char * filename)
{
    m_lib = SharedLibrary::load(filename);
	if (m_lib == nullptr)
		std::cout << "Error loading script: " << filename << std::endl;
	else
	{
		auto init = (Initializer)getMethodPtr("init");
		if (init != nullptr)
			init(m_allocator, getOwner());
	}
}

// ----------------------------------------------------------------------------

void CppScriptComponent::unload()
{
	auto clean = (Cleaner)getMethodPtr("clean");
	if (clean != nullptr)
		clean(getOwner());
    m_lib.reset();
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const CppScriptComponent & component)
{
	serializer.writeString("file", component.getFilename().c_str());
	return serializer;
}

// ============================================================================

void CppScriptComponentNamespace::warnMissing(const char * filename, const char * procedureName)
{
	static std::set<std::string> s_missingProcedures;

	std::string fileAndProcedure(filename);
	fileAndProcedure.append(procedureName);
	auto it = s_missingProcedures.find(fileAndProcedure);
	if (it == s_missingProcedures.end())
	{
		std::cout << "Error loading function " << procedureName << " in script " << filename << std::endl;
		s_missingProcedures.insert(fileAndProcedure);
	}
}

// ============================================================================
