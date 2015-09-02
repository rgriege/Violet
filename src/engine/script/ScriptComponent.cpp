// ============================================================================

#include "engine/script/ScriptComponent.h"

#include "engine/script/ScriptFactory.h"
#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag ScriptComponent::getStaticTag()
{
	return Tag('s', 'c', 'p', 't');
}

// ============================================================================

ScriptComponent::ScriptComponent(const Entity & owner, Deserializer & deserializer) :
	ComponentBase<ScriptComponent>(owner),
	m_script(ScriptFactory::create(deserializer.getString("file")))
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
