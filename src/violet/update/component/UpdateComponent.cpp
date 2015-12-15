// ============================================================================

#include "violet/update/component/UpdateComponent.h"

#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag UpdateComponent::getStaticTag()
{
	return Tag('u', 'p', 'd', 't');
}

// ----------------------------------------------------------------------------

Thread UpdateComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

UpdateComponent::UpdateComponent(const EntityId entityId) :
	ComponentBase<UpdateComponent>(entityId)
{
}

// ----------------------------------------------------------------------------

UpdateComponent::UpdateComponent(const EntityId entityId, ComponentDeserializer & /*deserializer*/) :
	ComponentBase<UpdateComponent>(entityId)
{
}

// ----------------------------------------------------------------------------

UpdateComponent::UpdateComponent(UpdateComponent && other) :
	ComponentBase<UpdateComponent>(std::move(other))
{
}

// ============================================================================

ComponentDeserializer & Violet::operator>>(ComponentDeserializer & deserializer, UpdateComponent & /*component*/)
{
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const UpdateComponent & /*component*/)
{
	return serializer;
}

// ============================================================================
