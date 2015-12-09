// ============================================================================

#include "violet/event/EventContext.h"

#include <algorithm>

using namespace Violet;

// ============================================================================

EventContext::EventContext() :
	m_subscriberGroups()
{
}

// ----------------------------------------------------------------------------

EventContext::EventContext(EventContext && other) :
	m_subscriberGroups(std::move(other.m_subscriberGroups))
{
}

// ----------------------------------------------------------------------------

void EventContext::subscribe(const uint32 eventId, const DelegateStore & func)
{
	m_subscriberGroups[eventId].emplace_back(func);
}

// ----------------------------------------------------------------------------

void EventContext::unsubscribe(const uint32 eventId, const DelegateStore & func)
{
	const auto subscriberGroupsIt = m_subscriberGroups.find(eventId);
	if (subscriberGroupsIt != m_subscriberGroups.end())
	{
		auto & subscribers = subscriberGroupsIt->second;
		const auto subscriberIt = std::find_if(subscribers.begin(), subscribers.end(), [=](const DelegateStore & subscriber) { return subscriber == func; });
		if (subscriberIt != subscribers.end())
			subscribers.erase(subscriberIt);
	}
}

// ============================================================================
