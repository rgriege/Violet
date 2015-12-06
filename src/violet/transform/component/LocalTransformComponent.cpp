// ============================================================================

#include "violet/transform/component/LocalTransformComponent.h"

#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag LocalTransformComponent::getStaticTag()
{
	return Tag('l', 't', 'f', 'm');
}

// ----------------------------------------------------------------------------

Thread LocalTransformComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

LocalTransformComponent::LocalTransformComponent(const Handle entityId) :
	LocalTransformComponent(entityId, Matrix3f::Identity)
{
}

// ----------------------------------------------------------------------------

LocalTransformComponent::LocalTransformComponent(const Handle entityId, Deserializer & deserializer) :
	LocalTransformComponent(entityId, Matrix3f::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

LocalTransformComponent::LocalTransformComponent(const Handle entityId, const Matrix3f & transform) :
	ComponentBase<LocalTransformComponent>(entityId),
	m_transform(transform)
{
}

// ----------------------------------------------------------------------------

LocalTransformComponent::LocalTransformComponent(LocalTransformComponent && other) :
	ComponentBase<LocalTransformComponent>(std::move(other)),
	m_transform(std::move(other.m_transform))
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, LocalTransformComponent & component)
{
	return deserializer >> component.m_transform;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const LocalTransformComponent & component)
{
	return serializer << component.m_transform;
}

// ============================================================================
