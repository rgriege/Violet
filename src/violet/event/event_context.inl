// ============================================================================

#include <functional>

// ============================================================================

template <typename ... Args>
void vlt::event_context::emit(const u32 event_id, Args && ... args) const
{
	const auto it = subscriber_groups.find(event_id);
	if (it != subscriber_groups.end())
	{
		for (const auto & subscriber : it->second)
		{
			delegate<void(Args...)> delegate(subscriber);
			delegate(std::forward<Args>(args)...);
		}
	}
}

// ============================================================================
