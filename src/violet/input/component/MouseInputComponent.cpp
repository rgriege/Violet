// ============================================================================

#include "violet/input/component/MouseInputComponent.h"

#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag MouseInputComponent::getStaticTag()
{
	return Tag('m', 'i', 'p', 't');
}

// ============================================================================

MouseInputComponent::MouseInputComponent(Entity & owner, Deserializer & deserializer) :
	ComponentBase<MouseInputComponent>(owner),
	m_mesh(deserializer)
{
}

// ----------------------------------------------------------------------------

MouseInputComponent::MouseInputComponent(Entity & owner, Polygon && mesh) :
	ComponentBase<MouseInputComponent>(owner),
	m_mesh(std::move(mesh))
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
