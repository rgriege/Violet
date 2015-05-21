// ============================================================================

#include "engine/transform/component/TransformComponent.h"

#include "engine/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag TransformComponent::getTypeId()
{
	return Tag('t', 's', 'f', 'm');
}

// ============================================================================

TransformComponent::TransformComponent(const Entity entity) :
	TransformComponent(entity, Vec2f::ZERO, 0)
{
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(const Entity entity, Deserializer & deserializer) :
	TransformComponent(entity, Vec2f::ZERO, 0)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(const Entity entity, const Vec2f position, const float rotation) :
	Component<TransformComponent>(entity),
	m_position(position),
	m_rotation(rotation)
{
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(TransformComponent && other) :
	Component<TransformComponent>(std::move(other)),
	m_position(std::move(other.m_position)),
	m_rotation(other.m_rotation)
{
}

// ----------------------------------------------------------------------------

TransformComponent & TransformComponent::operator=(TransformComponent && other)
{
	Component<TransformComponent>::operator=(std::move(other));
	m_position = std::move(other.m_position);
	m_rotation = other.m_rotation;
	return *this;
}

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
