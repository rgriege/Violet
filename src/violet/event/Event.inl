// ============================================================================

template <typename Derived, typename ... Args>
uint32 Violet::Event<Derived, void(Args...)>::subscribe(EventContext & eventContext, const Delegate & func)
{
	return eventContext.subscribe(Derived::getName(), new Delegate(func));
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::emit(const EventContext & eventContext, Args && ... args)
{
	eventContext.emit(Derived::getName(), std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::unsubscribe(EventContext & eventContext, const uint32 delegateId)
{
	eventContext.unsubscribe(Derived::getName(), delegateId);
}

// ============================================================================
