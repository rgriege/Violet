#include "violet/core/entity/Entity.h"

using namespace Violet;

namespace EntityNamespace
{
	uint32 s_id;
}

using namespace EntityNamespace;

// ============================================================================

Entity::Entity() :
	id(++s_id)
{
}

// ============================================================================
