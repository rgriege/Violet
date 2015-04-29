#include "violet/core/script/system/CppScriptSystem.h"

using namespace Violet;

void CppScriptSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &CppScriptSystem::init);
}

std::unique_ptr<System> CppScriptSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	auto system = new CppScriptSystem;
	return std::unique_ptr<System>(system);
}

CppScriptSystem::CppScriptSystem(CppScriptSystem && other) :
	ComponentSystem<CppScriptComponent>(std::move(other))
{
}

void CppScriptSystem::update(float /*dt*/, Engine & /*engine*/)
{
}