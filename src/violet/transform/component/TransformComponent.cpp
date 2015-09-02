// ============================================================================

#include "violet/transform/component/TransformComponent.h"

#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag TransformComponent::getStaticTag()
{
	return Tag('t', 's', 'f', 'm');
}

// ============================================================================

TransformComponent::TransformComponent(const Entity & owner) :
	TransformComponent(owner, Vec2f::ZERO, 0)
{
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(const Entity & owner, Deserializer & deserializer) :
	TransformComponent(owner, Vec2f::ZERO, 0)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(const Entity & owner, const Vec2f position, const float rotation) :
	ComponentBase<TransformComponent>(owner),
	m_position(position),
	m_rotation(rotation)
{
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(TransformComponent && other) :
	ComponentBase<TransformComponent>(std::move(other)),
	m_position(std::move(other.m_position)),
	m_rotation(other.m_rotation)
{
}

// ----------------------------------------------------------------------------

//TransformComponent & TransformComponent::operator=(TransformComponent && other)
//{
//	Component::operator=(std::move(other));
//	m_position = std::move(other.m_position);
//	m_rotation = other.m_rotation;
//	return *this;
//}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, TransformComponent & component)
{
	return deserializer >> component.m_position;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const TransformComponent & component)
{
	return serializer << component.m_position;
}

// ============================================================================
