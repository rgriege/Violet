// ============================================================================

#include "violet/update/component/UpdateComponent.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag UpdateComponent::getStaticTag()
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

// ----------------------------------------------------------------------------

UpdateComponent::UpdateComponent(UpdateComponent && other) :
	ComponentBase<UpdateComponent>(std::move(other))
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
