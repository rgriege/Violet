// ============================================================================

#include "violet/handle/HandleComponent.h"

#include "violet/Engine.h"
#include "violet/entity/Entity.h"
#include "violet/scene/Scene.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag HandleComponent::getStaticTag()
{
	return Tag('h', 'n', 'd', 'l');
}

// ============================================================================

HandleComponent::HandleComponent(Entity & owner) :
	ComponentBase<HandleComponent>(owner),
	m_handle()
{
	owner.getScene().index(*this);
}

// ----------------------------------------------------------------------------

HandleComponent::HandleComponent(Entity & owner, Deserializer & deserializer) :
	ComponentBase<HandleComponent>(owner),
	m_handle()
{
	deserializer >> *this;
	owner.getScene().index(*this);
}

// ----------------------------------------------------------------------------

HandleComponent::HandleComponent(HandleComponent && other) :
	ComponentBase<HandleComponent>(std::move(other)),
	m_handle(std::move(other.m_handle))
{
	other.m_handle = Handle::ms_invalid;
}

// ----------------------------------------------------------------------------

HandleComponent::~HandleComponent()
{
	if (m_handle.isValid())
		m_owner.getScene().deindex(*this);
}

// ----------------------------------------------------------------------------

Handle HandleComponent::getHandle() const
{
	return m_handle;
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, HandleComponent & component)
{
	component.m_handle = Handle(deserializer.getUint("id"), 0);
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const HandleComponent & component)
{
	serializer.writeUint("id", component.m_handle.getId());
	return serializer;
}

// ============================================================================
