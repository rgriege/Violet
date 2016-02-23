// ============================================================================

#include "violet/event/event_context_owner.h"

using namespace vlt;

// ============================================================================

Event_Context_Owner::Event_Context_Owner() :
	event_context()
{
}

// ----------------------------------------------------------------------------

Event_Context_Owner::Event_Context_Owner(Event_Context_Owner && other) :
	event_context(std::move(other.event_context))
{
}

// ============================================================================
