// ============================================================================

#include "violet/component/Component.h"
#include "violet/entity/Entity.h"

using namespace Violet;

// ============================================================================

Entity::Entity() :
	m_components(),
	m_componentFlags()
{
}

// ----------------------------------------------------------------------------

uint32 Entity::getComponentFlags() const
{
	return m_componentFlags;
}

// ============================================================================
