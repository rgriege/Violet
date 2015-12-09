// ============================================================================

#include "violet/Engine.h"

// ============================================================================

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::subscribe(EventContext & eventContext, const Subscriber & func)
{
	eventContext.subscribe(Derived::getIdentifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::subscribe(const EventContext & eventContext, const Subscriber & func)
{
	Violet::EventBase::subscribe(eventContext, Derived::getIdentifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::subscribe(EventContextOwner & eventContextOwner, const Subscriber & func)
{
	subscribe(eventContextOwner.getEventContext(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::subscribe(const EventContextOwner & eventContextOwner, const Subscriber & func)
{
	subscribe(eventContextOwner.getEventContext(), func);
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
void Violet::Event<Derived, void(Args...)>::unsubscribe(EventContext & eventContext, const Subscriber & func)
{
	eventContext.unsubscribe(Derived::getIdentifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::unsubscribe(const EventContext & eventContext, const Subscriber & func)
{
	Violet::EventBase::unsubscribe(eventContext, Derived::getIdentifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::unsubscribe(EventContextOwner & eventContextOwner, const Subscriber & func)
{
	unsubscribe(eventContextOwner.getEventContext(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void Violet::Event<Derived, void(Args...)>::unsubscribe(const EventContextOwner & eventContextOwner, const Subscriber & func)
{
	unsubscribe(eventContextOwner.getEventContext(), func);
}

// ============================================================================
