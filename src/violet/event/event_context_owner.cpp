// ============================================================================

#include "violet/event/event_context_owner.h"

using namespace vlt;

// ============================================================================

event_context_owner::event_context_owner() :
	event_context()
{
}

// ----------------------------------------------------------------------------

event_context_owner::event_context_owner(event_context_owner && other) :
	event_context(std::move(other.event_context))
{
}

// ============================================================================
