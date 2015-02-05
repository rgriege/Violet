#include "violet/core/entity/Entity.h"

using namespace Violet;

// ============================================================================

Entity::Entity(uint32 id) :
	m_id(id)
{
}

Entity::Entity(const Entity & other) :
	m_id(other.m_id)
{
}

// ============================================================================
