// ============================================================================

#include "engine/update/component/UpdateComponent.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag UpdateComponent::getTypeId()
{
	return Tag('u', 'p', 'd', 't');
}

// ============================================================================

UpdateComponent::UpdateComponent(const Entity entity) :
	Component<UpdateComponent>(entity)
{
}

UpdateComponent::UpdateComponent(const Entity entity, Deserializer & /*deserializer*/) :
	Component<UpdateComponent>(entity)
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, UpdateComponent & /*component*/)
{
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const UpdateComponent & /*component*/)
{
	return serializer;
}

// ============================================================================
