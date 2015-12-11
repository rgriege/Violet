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

// ----------------------------------------------------------------------------

Thread ScriptComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

ScriptComponent::ScriptComponent(const EntityId entityId, Deserializer & deserializer) :
	ScriptComponent(entityId, deserializer.getString("file"))
{
}

// ----------------------------------------------------------------------------

ScriptComponent::ScriptComponent(EntityId entityId, const char * const fileName) :
	ComponentBase<ScriptComponent>(entityId),
	m_script(ScriptFactory::create(fileName))
{
	BindToComponentMethod::run(*m_script, std::move(entityId));
}

// ----------------------------------------------------------------------------

ScriptComponent::ScriptComponent(ScriptComponent && other) :
	ComponentBase<ScriptComponent>(std::move(other)),
	m_script(std::move(other.m_script))
{
}

// ----------------------------------------------------------------------------

ScriptComponent::~ScriptComponent()
{
	UnbindFromComponentMethod::run(*m_script, std::move(m_entityId));
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const ScriptComponent & component)
{
	serializer.writeString("file", component.m_script->getFileName().c_str());
	return serializer;
}

// ============================================================================
