#include "engine/entity/Entity.h"

using namespace Violet;

// ============================================================================

Entity::Entity(uint32 id) :
	m_id(id)
{
}

uint32 Entity::getId() const
{
	return m_id;
}

// ============================================================================
