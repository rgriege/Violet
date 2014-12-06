#include "violet/core/entity/Entity.h"

using namespace Violet;

namespace EntityNamespace
{
	uint32 ms_id;
}

using namespace EntityNamespace;

// ============================================================================

Entity::Entity() :
	m_id(ms_id++)
{
}

// ============================================================================
