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

LocalTransformComponent::LocalTransformComponent(const EntityId entityId, Deserializer & deserializer) :
	LocalTransformComponent(entityId, EntityId::ms_invalid, Matrix3f::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

LocalTransformComponent::LocalTransformComponent(const EntityId entityId, const EntityId parentId, const Matrix3f & transform) :
	ComponentBase<LocalTransformComponent>(entityId),
	m_parentId(parentId),
	m_transform(transform)
{
}

// ----------------------------------------------------------------------------

LocalTransformComponent::LocalTransformComponent(LocalTransformComponent && other) :
	ComponentBase<LocalTransformComponent>(std::move(other)),
	m_parentId(other.m_parentId),
	m_transform(std::move(other.m_transform))
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, LocalTransformComponent & component)
{
	component.m_parentId = EntityId(deserializer.getUint("parentId"), component.getEntityId().getVersion());
	return deserializer >> component.m_transform;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const LocalTransformComponent & component)
{
	serializer.writeUint("parentId", component.m_parentId.getId());
	return serializer << component.m_transform;
}

// ============================================================================
