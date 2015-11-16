// ============================================================================

#include <functional>

// ============================================================================

template <typename ... Args>
void Violet::EventContext::emit(const char * const eventName, Args && ... args) const
{
	const auto subscriberGroupsIt = m_subscriberGroups.find(eventName);
	if (subscriberGroupsIt != m_subscriberGroups.end())
	{
		const auto & subscribers = subscriberGroupsIt->second.m_subscribers;
		for (auto const & subscriber : subscribers)
			(*static_cast<std::function<void(Args...)>*>(subscriber.m_delegate))(std::forward<Args>(args)...);
	}
}

// ============================================================================
