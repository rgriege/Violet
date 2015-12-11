// ============================================================================

#include "violet/transform/component/WorldTransformComponent.h"

#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag WorldTransformComponent::getStaticTag()
{
	return Tag('w', 't', 'f', 'm');
}

// ----------------------------------------------------------------------------

Thread WorldTransformComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

WorldTransformComponent::WorldTransformComponent(const EntityId entityId) :
	WorldTransformComponent(entityId, Matrix3f::Identity)
{
}

// ----------------------------------------------------------------------------

WorldTransformComponent::WorldTransformComponent(const EntityId entityId, Deserializer & deserializer) :
	WorldTransformComponent(entityId, Matrix3f::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

WorldTransformComponent::WorldTransformComponent(const EntityId entityId, const Matrix3f & transform) :
	ComponentBase<WorldTransformComponent>(entityId),
	m_transform(transform)
{
}

// ----------------------------------------------------------------------------

WorldTransformComponent::WorldTransformComponent(WorldTransformComponent && other) :
	ComponentBase<WorldTransformComponent>(std::move(other)),
	m_transform(std::move(other.m_transform))
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, WorldTransformComponent & component)
{
	return deserializer >> component.m_transform;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const WorldTransformComponent & component)
{
	return serializer << component.m_transform;
}

// ============================================================================
