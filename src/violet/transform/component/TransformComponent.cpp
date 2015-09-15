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

TransformComponent::TransformComponent(Entity & owner) :
	TransformComponent(owner, Matrix3f::Identity)
{
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(Entity & owner, Deserializer & deserializer) :
	TransformComponent(owner, Matrix3f::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(Entity & owner, const Matrix3f & transform) :
	ComponentBase<TransformComponent>(owner),
	m_transform(transform)
{
}

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(TransformComponent && other) :
	ComponentBase<TransformComponent>(std::move(other)),
	m_transform(std::move(other.m_transform))
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, TransformComponent & component)
{
	return deserializer >> component.m_transform;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const TransformComponent & component)
{
	return serializer << component.m_transform;
}

// ============================================================================
