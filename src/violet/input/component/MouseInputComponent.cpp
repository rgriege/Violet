// ============================================================================

#include "violet/input/component/MouseInputComponent.h"

#include "violet/component/ComponentDeserializer.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag MouseInputComponent::getStaticTag()
{
	return Tag('m', 'i', 'p', 't');
}

// ----------------------------------------------------------------------------

Thread MouseInputComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

MouseInputComponent::MouseInputComponent(const EntityId entityId, ComponentDeserializer & deserializer) :
	ComponentBase<MouseInputComponent>(entityId),
	m_mesh(deserializer)
{
}

// ----------------------------------------------------------------------------

MouseInputComponent::MouseInputComponent(const EntityId entityId, Polygon && mesh) :
	ComponentBase<MouseInputComponent>(entityId),
	m_mesh(std::move(mesh))
{
}

// ----------------------------------------------------------------------------

MouseInputComponent::MouseInputComponent(MouseInputComponent && other) :
	ComponentBase<MouseInputComponent>(std::move(other)),
	m_mesh(std::move(other.m_mesh))
{
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const MouseInputComponent & component)
{
	return serializer << component.m_mesh;
}

// ----------------------------------------------------------------------------

ComponentDeserializer & Violet::operator>>(ComponentDeserializer & deserializer, MouseInputComponent & component)
{
	deserializer >> component.m_mesh;
	return deserializer;
}

// ============================================================================
