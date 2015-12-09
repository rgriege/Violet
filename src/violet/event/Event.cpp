// ============================================================================

#include "violet/event/Event.h"

#include <algorithm>

using namespace Violet;

// ============================================================================

void EventBase::subscribe(const EventContext & eventContext, const uint32 identifier, const DelegateStore & func)
{
	Violet::Engine::getInstance().addWriteTask(eventContext,
		[=](EventContext & eventContext)
		{
			eventContext.subscribe(identifier, func);
		});
}

// ----------------------------------------------------------------------------

void EventBase::unsubscribe(const EventContext & eventContext, const uint32 identifier, const DelegateStore & func)
{
	Violet::Engine::getInstance().addWriteTask(eventContext,
		[=](EventContext & eventContext)
	{
		eventContext.unsubscribe(identifier, func);
	});
}

// ============================================================================
