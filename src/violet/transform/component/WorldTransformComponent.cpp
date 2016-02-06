// ============================================================================

#include "violet/transform/component/WorldTransformComponent.h"

#include "violet/component/ComponentDeserializer.h"
#include "violet/serialization/Serializer.h"

#include <assert.h>

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
	WorldTransformComponent(entityId, Matrix4f::Identity)
{
}

// ----------------------------------------------------------------------------

WorldTransformComponent::WorldTransformComponent(const EntityId entityId, ComponentDeserializer & deserializer) :
	WorldTransformComponent(entityId, Matrix4f::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

WorldTransformComponent::WorldTransformComponent(const EntityId entityId, const Matrix4f & transform) :
	ComponentBase<WorldTransformComponent, 1>(),
	m_transform(transform)
{
}

// ============================================================================

ComponentDeserializer & Violet::operator>>(ComponentDeserializer & deserializer, WorldTransformComponent & component)
{
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

Serializer & Violet::operator<<(Serializer & serializer, const WorldTransformComponent & component)
{
	return serializer << component.m_transform;
}

// ============================================================================
