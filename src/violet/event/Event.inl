// ============================================================================

#include "violet/Engine.h"

// ============================================================================

template <typename Derived, typename ... Args>
uint32 Violet::Event<Derived, void(Args...)>::subscribe(EventContext & eventContext, const Delegate & func)
{
	return eventContext.subscribe(Derived::getIdentifier(), new Delegate(func));
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
uint32 Violet::Event<Derived, void(Args...)>::subscribe(EventContextOwner & eventContextOwner, const Delegate & func)
{
	return subscribe(eventContextOwner.getEventContext(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::emit(const EventContext & eventContext, Args && ... args)
{
	eventContext.emit(Derived::getIdentifier(), std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::emit(const EventContextOwner & eventContextOwner, Args && ... args)
{
	emit(eventContextOwner.getEventContext(), std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::unsubscribe(EventContext & eventContext, const uint32 delegateId)
{
	eventContext.unsubscribe(Derived::getIdentifier(), delegateId);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::unsubscribe(const EventContext & eventContext, const uint32 delegateId)
{
	Violet::Engine::getInstance().addWriteTask(eventContext,
		[=](EventContext & eventContext)
		{
			eventContext.unsubscribe(Derived::getIdentifier(), delegateId);
		});
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::unsubscribe(EventContextOwner & eventContextOwner, const uint32 delegateId)
{
	unsubscribe(eventContextOwner.getEventContext(), delegateId);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::unsubscribe(const EventContextOwner & eventContextOwner, const uint32 delegateId)
{
	unsubscribe(eventContextOwner.getEventContext(), delegateId);
}

// ============================================================================
