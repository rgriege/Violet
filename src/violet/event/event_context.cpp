// ============================================================================

#include <algorithm>

#include "violet/event/event_context.h"

using namespace vlt;

// ============================================================================

Event_Context::Event_Context() :
	subscriber_groups()
{
}

// ----------------------------------------------------------------------------

Event_Context::Event_Context(Event_Context && other) :
	subscriber_groups(std::move(other.subscriber_groups))
{
}

// ----------------------------------------------------------------------------

void Event_Context::subscribe(const u32 event_id, const Delegate_Store & func)
{
	subscriber_groups[event_id].emplace_back(func);
}

// ----------------------------------------------------------------------------

void Event_Context::unsubscribe(const u32 event_id, const Delegate_Store & func)
{
	const auto it = subscriber_groups.find(event_id);
	if (it != subscriber_groups.end())
	{
		auto & subscribers = it->second;
		const auto subscriberIt = std::find_if(subscribers.begin(), subscribers.end(), [=](const Delegate_Store & subscriber) { return subscriber == func; });
		if (subscriberIt != subscribers.end())
			subscribers.erase(subscriberIt);
	}
}

// ============================================================================
