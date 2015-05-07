// ============================================================================

#include "engine/entity/Entity.h"

using namespace Violet;

// ============================================================================

Entity Entity::INVALID(~0, ~0);

// ============================================================================

Entity::Entity(const uint32 id, const uint32 version) :
	m_id(id),
	m_version(version)
{
}

// ----------------------------------------------------------------------------

uint32 Entity::getId() const
{
	return m_id;
}

// ----------------------------------------------------------------------------

uint32 Entity::getVersion() const
{
	return m_version;
}

// ============================================================================

bool Violet::operator<(const Entity lhs, const Entity rhs)
{
	return lhs.getId() < rhs.getId();
}

// ----------------------------------------------------------------------------

bool Violet::operator==(Entity lhs, Entity rhs)
{
	return lhs.getId() == rhs.getId() && lhs.getVersion() == rhs.getVersion();
}

// ----------------------------------------------------------------------------

bool Violet::operator!=(Entity lhs, Entity rhs)
{
	return !(lhs == rhs);
}

// ============================================================================
