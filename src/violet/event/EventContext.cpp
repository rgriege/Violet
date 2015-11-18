// ============================================================================

#include "violet/event/EventContext.h"

#include <algorithm>

using namespace Violet;

// ============================================================================

EventContext::Subscriber::Subscriber(const uint32 id, void * delegate) :
	m_id(id),
	m_delegate(delegate)
{
}

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

uint32 EventContext::subscribe(const uint32 eventId, void * delegate)
{
	auto & subscriberGroup = m_subscriberGroups[eventId];
	subscriberGroup.m_subscribers.emplace_back(subscriberGroup.m_idList.reserve(), delegate);
	return subscriberGroup.m_subscribers.back().m_id;
}

// ----------------------------------------------------------------------------

void EventContext::unsubscribe(const uint32 eventId, const uint32 delegateId)
{
	const auto subscriberGroupsIt = m_subscriberGroups.find(eventId);
	if (subscriberGroupsIt != m_subscriberGroups.end())
	{
		auto & subscriberGroup = subscriberGroupsIt->second;
		auto & subscribers = subscriberGroup.m_subscribers;
		const auto subscriberIt = std::find_if(subscribers.begin(), subscribers.end(), [=](const Subscriber & subscriber) { return subscriber.m_id == delegateId; });
		if (subscriberIt != subscribers.end())
		{
			subscribers.erase(subscriberIt);
			subscriberGroup.m_idList.free(delegateId);
		}
	}
}

// ============================================================================
