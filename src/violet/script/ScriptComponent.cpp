// ============================================================================

#include "violet/script/ScriptComponent.h"

#include "violet/script/ScriptFactory.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag ScriptComponent::getStaticTag()
{
	return Tag('s', 'c', 'p', 't');
}

// ============================================================================

ScriptComponent::ScriptComponent(const Entity & owner, Deserializer & deserializer) :
	ScriptComponent(owner, deserializer.getString("file"))
{
}

// ----------------------------------------------------------------------------

ScriptComponent::ScriptComponent(const Entity & owner, const char * const fileName) :
	ComponentBase<ScriptComponent>(owner),
	m_script(ScriptFactory::create(fileName))
{

}

// ----------------------------------------------------------------------------

ScriptComponent::ScriptComponent(ScriptComponent && other) :
	ComponentBase<ScriptComponent>(std::move(other)),
	m_script(std::move(other.m_script))
{
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const ScriptComponent & component)
{
	serializer.writeString("file", component.m_script->getFileName().c_str());
	return serializer;
}

// ============================================================================
