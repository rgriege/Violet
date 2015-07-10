// ============================================================================

#include "engine/input/component/MouseInputComponent.h"

#include "engine/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag MouseInputComponent::getTag()
{
	return Tag('m', 'i', 'p', 't');
}

// ============================================================================

MouseInputComponent::MouseInputComponent(const Entity & owner, Deserializer & deserializer) :
	ComponentBase<MouseInputComponent>(owner),
	m_mesh(deserializer)
{
}

// ----------------------------------------------------------------------------

MouseInputComponent::MouseInputComponent(MouseInputComponent && other) :
	ComponentBase<MouseInputComponent>(std::move(other)),
	m_mesh(std::move(other.m_mesh))
{
}

// ----------------------------------------------------------------------------

//MouseInputComponent & MouseInputComponent::operator=(MouseInputComponent && other)
//{
//	m_mesh = std::move(other.m_mesh);
//	return *this;
//}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const MouseInputComponent & component)
{
	return serializer << component.m_mesh;
}

// ----------------------------------------------------------------------------

Deserializer & Violet::operator>>(Deserializer & deserializer, MouseInputComponent & component)
{
	return deserializer >> component.m_mesh;
}

// ============================================================================
