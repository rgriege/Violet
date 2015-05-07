// ============================================================================

#include "engine/component/Component.h"

using namespace Violet;

// ============================================================================

BaseComponent::~BaseComponent()
{
}

// ============================================================================

BaseComponent::BaseComponent(const Entity entity) :
	m_entity(entity)
{
}

// ----------------------------------------------------------------------------

BaseComponent::BaseComponent(BaseComponent && other) :
	m_entity(other.m_entity)
{
}

// ----------------------------------------------------------------------------

BaseComponent & BaseComponent::operator=(BaseComponent && other)
{
	m_entity = other.m_entity;
	return *this;
}

// ----------------------------------------------------------------------------

Entity BaseComponent::getEntity() const
{
	return m_entity;
}

// ============================================================================
