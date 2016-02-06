// ============================================================================

#include "violet/transform/component/LocalTransformComponent.h"

#include "violet/component/ComponentDeserializer.h"
#include "violet/serialization/Serializer.h"

#include <assert.h>

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
	LocalTransformComponent(entityId, EntityId::ms_invalid, Matrix4f::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

LocalTransformComponent::LocalTransformComponent(const EntityId entityId, const EntityId parentId, const Matrix4f & transform) :
	ComponentBase<LocalTransformComponent, 1>(),
	m_parentId(parentId),
	m_transform(transform)
{
}

// ============================================================================

ComponentDeserializer & Violet::operator>>(ComponentDeserializer & deserializer, LocalTransformComponent & component)
{
	component.m_parentId = deserializer.getEntityId("parentId");

	switch (deserializer.getVersion())
	{
	case 0:
		Matrix3f mat2d;
		deserializer >> mat2d;
		component.m_transform = from2d(mat2d);
		break;

	case 1:
		deserializer >> component.m_transform;
		break;

	default:
		assert(false);
		break;
	}

	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const LocalTransformComponent & component)
{
	serializer.writeUint("parentId", component.m_parentId.getId());
	return serializer << component.m_transform;
}

// ============================================================================
