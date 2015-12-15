// ============================================================================

#include "violet/transform/component/LocalTransformComponent.h"

#include "violet/component/ComponentDeserializer.h"
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

LocalTransformComponent::LocalTransformComponent(const EntityId entityId, ComponentDeserializer & deserializer) :
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

ComponentDeserializer & Violet::operator>>(ComponentDeserializer & deserializer, LocalTransformComponent & component)
{
	component.m_parentId = deserializer.getEntityId("parentId");
	deserializer >> component.m_transform;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const LocalTransformComponent & component)
{
	serializer.writeUint("parentId", component.m_parentId.getId());
	return serializer << component.m_transform;
}

// ============================================================================
