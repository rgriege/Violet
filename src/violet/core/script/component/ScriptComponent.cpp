#include "violet/core/script/component/ScriptComponent.h"

#include "violet/core/script/ScriptFactory.h"
#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

const char * ScriptComponent::getLabel()
{
	return "scpt";
}

ScriptComponent::ScriptComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_script(ScriptFactory::getInstance().create(deserializer.getString("file")))
{
}

ScriptComponent::ScriptComponent(ScriptComponent && other) :
	Component(std::move(other)),
	m_script(std::move(other.m_script))
{
}

ScriptComponent & ScriptComponent::operator=(ScriptComponent && other)
{
	std::swap(m_script, other.m_script);
	return *this;
}