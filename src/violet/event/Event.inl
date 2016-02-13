// ============================================================================

#include "violet/core/engine.h"

// ============================================================================

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::subscribe(event_context & event_context, const subscriber & func)
{
	event_context.subscribe(Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::subscribe(const event_context & event_context, const subscriber & func)
{
	vlt::event_base::subscribe(event_context, Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::subscribe(event_context_owner & event_context_owner, const subscriber & func)
{
	subscribe(event_context_owner.event_context, func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::subscribe(const event_context_owner & event_context_owner, const subscriber & func)
{
	subscribe(event_context_owner.event_context, func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::emit(const event_context & event_context, Args && ... args)
{
	event_context.emit(Derived::get_identifier(), std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::emit(const event_context_owner & event_context_owner, Args && ... args)
{
	emit(event_context_owner.event_context, std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::unsubscribe(event_context & event_context, const subscriber & func)
{
	event_context.unsubscribe(Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::unsubscribe(const event_context & event_context, const subscriber & func)
{
	vlt::event_base::unsubscribe(event_context, Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::unsubscribe(event_context_owner & event_context_owner, const subscriber & func)
{
	unsubscribe(event_context_owner.event_context, func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ... Args>
void vlt::event<Derived, void(Args...)>::unsubscribe(const event_context_owner & event_context_owner, const subscriber & func)
{
	unsubscribe(event_context_owner.event_context, func);
}

// ============================================================================
