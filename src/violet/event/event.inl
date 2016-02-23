// ============================================================================

#include "violet/core/engine.h"

// ============================================================================

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::subscribe(Event_Context & context, const Subscriber & func)
{
	context.subscribe(Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::subscribe(const Event_Context & context, const Subscriber & func)
{
	vlt::Event_Base::subscribe(context, Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::subscribe(Event_Context_Owner & owner, const Subscriber & func)
{
	subscribe(owner.event_context, func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::subscribe(const Event_Context_Owner & owner, const Subscriber & func)
{
	subscribe(owner.event_context, func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::emit(const Event_Context & context, Args && ... args)
{
	context.emit(Derived::get_identifier(), std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::emit(const Event_Context_Owner & owner, Args && ... args)
{
	emit(owner.event_context, std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::unsubscribe(Event_Context & context, const Subscriber & func)
{
	context.unsubscribe(Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::unsubscribe(const Event_Context & context, const Subscriber & func)
{
	vlt::Event_Base::unsubscribe(context, Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::unsubscribe(Event_Context_Owner & owner, const Subscriber & func)
{
	unsubscribe(owner.event_context, func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::Event<Derived, void(Args...)>::unsubscribe(const Event_Context_Owner & owner, const Subscriber & func)
{
	unsubscribe(owner.event_context, func);
}

// ============================================================================
