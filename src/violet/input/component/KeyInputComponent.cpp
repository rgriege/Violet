// ============================================================================

#include "violet/input/component/KeyInputComponent.h"

#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag KeyInputComponent::getStaticTag()
{
	return Tag('k', 'i', 'p', 't');
}

// ============================================================================

KeyInputComponent::KeyInputComponent(Entity & owner, Deserializer & deserializer) :
	ComponentBase<KeyInputComponent>(owner)
{
}

// ----------------------------------------------------------------------------

KeyInputComponent::KeyInputComponent(KeyInputComponent && other) :
	ComponentBase<KeyInputComponent>(std::move(other))
{
}

// ----------------------------------------------------------------------------

//KeyInputComponent & KeyInputComponent::operator=(KeyInputComponent && other)
//{
//	m_mesh = std::move(other.m_mesh);
//	return *this;
//}

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
