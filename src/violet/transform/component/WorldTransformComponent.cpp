// ============================================================================

#include "violet/transform/component/WorldTransformComponent.h"

#include "violet/component/ComponentDeserializer.h"
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

WorldTransformComponent::WorldTransformComponent(const EntityId entityId, ComponentDeserializer & deserializer) :
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

// ============================================================================

ComponentDeserializer & Violet::operator>>(ComponentDeserializer & deserializer, WorldTransformComponent & component)
{
	deserializer >> component.m_transform;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const WorldTransformComponent & component)
{
	return serializer << component.m_transform;
}

// ============================================================================
