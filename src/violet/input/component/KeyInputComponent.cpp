// ============================================================================

#include "violet/input/component/KeyInputComponent.h"

#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag KeyInputComponent::getStaticTag()
{
	return Tag('k', 'i', 'p', 't');
}

// ----------------------------------------------------------------------------

Thread KeyInputComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

KeyInputComponent::KeyInputComponent(const EntityId entityId) :
	ComponentBase<KeyInputComponent>(entityId)
{
}

// ----------------------------------------------------------------------------

KeyInputComponent::KeyInputComponent(const EntityId entityId, Deserializer & deserializer) :
	KeyInputComponent(entityId)
{
}

// ----------------------------------------------------------------------------

KeyInputComponent::KeyInputComponent(KeyInputComponent && other) :
	ComponentBase<KeyInputComponent>(std::move(other))
{
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const KeyInputComponent & component)
{
	return serializer;
}

// ----------------------------------------------------------------------------

Deserializer & Violet::operator>>(Deserializer & deserializer, KeyInputComponent & component)
{
	return deserializer;
}

// ============================================================================
