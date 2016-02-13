// ============================================================================

#include <algorithm>

#include "violet/event/event_context.h"

using namespace vlt;

// ============================================================================

event_context::event_context() :
	subscriber_groups()
{
}

// ----------------------------------------------------------------------------

event_context::event_context(event_context && other) :
	subscriber_groups(std::move(other.subscriber_groups))
{
}

// ----------------------------------------------------------------------------

void event_context::subscribe(const u32 eventId, const delegate_store & func)
{
	subscriber_groups[eventId].emplace_back(func);
}

// ----------------------------------------------------------------------------

void event_context::unsubscribe(const u32 eventId, const delegate_store & func)
{
	const auto it = subscriber_groups.find(eventId);
	if (it != subscriber_groups.end())
	{
		auto & subscribers = it->second;
		const auto subscriberIt = std::find_if(subscribers.begin(), subscribers.end(), [=](const delegate_store & subscriber) { return subscriber == func; });
		if (subscriberIt != subscribers.end())
			subscribers.erase(subscriberIt);
	}
}

// ============================================================================
