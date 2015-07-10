// ============================================================================

#include "engine/update/component/UpdateComponent.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag UpdateComponent::getTag()
{
	return Tag('u', 'p', 'd', 't');
}

// ============================================================================

UpdateComponent::UpdateComponent(const Entity & owner) :
	ComponentBase<UpdateComponent>(owner)
{
}

// ----------------------------------------------------------------------------

UpdateComponent::UpdateComponent(const Entity & owner, Deserializer & /*deserializer*/) :
	ComponentBase<UpdateComponent>(owner)
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
