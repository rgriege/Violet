// ============================================================================

#include "engine/input/component/InputComponent.h"

#include "engine/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag InputComponent::getTypeId()
{
	return Tag('i', 'n', 'p', 't');
}

// ============================================================================

InputComponent::InputComponent(const Entity entity, Deserializer & deserializer) :
	Component(entity),
	m_mesh(deserializer)
{
}

// ----------------------------------------------------------------------------

InputComponent::InputComponent(InputComponent && other) :
	Component(std::move(other)),
	m_mesh(std::move(other.m_mesh))
{
}

// ----------------------------------------------------------------------------

InputComponent & InputComponent::operator=(InputComponent && other)
{
	m_mesh = std::move(other.m_mesh);
	return *this;
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const InputComponent & component)
{
	return serializer << component.m_mesh;
}

// ----------------------------------------------------------------------------

Deserializer & Violet::operator>>(Deserializer & deserializer, InputComponent & component)
{
	return deserializer >> component.m_mesh;
}

// ============================================================================