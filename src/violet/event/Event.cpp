// ============================================================================

#include "violet/event/Event.h"

#include <algorithm>

using namespace Violet;

// ============================================================================

void EventBase::unsubscribe(const EventContext & eventContext, const uint32 identifier, const uint32 delegateId)
{
	Violet::Engine::getInstance().addWriteTask(eventContext,
		[=](EventContext & eventContext)
		{
			eventContext.unsubscribe(identifier, delegateId);
		});
}

// ============================================================================
