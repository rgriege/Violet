// ============================================================================

#include <functional>

// ============================================================================

template <typename ... Args>
void Violet::EventContext::emit(const uint32 eventId, Args && ... args) const
{
	const auto subscriberGroupsIt = m_subscriberGroups.find(eventId);
	if (subscriberGroupsIt != m_subscriberGroups.end())
	{
		for (const auto & subscriber : subscriberGroupsIt->second)
        {
			Delegate<void(Args...)> delegate(subscriber);
            delegate(std::forward<Args>(args)...);
        }
	}
}

// ============================================================================
