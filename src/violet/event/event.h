#ifndef VIOLET_EVENT_H
#define VIOLET_EVENT_H

#include "violet/event/event_context.h"
#include "violet/event/event_context_owner.h"
#include "violet/utility/delegate.h"

namespace vlt
{
	struct VIOLET_API Event_Base
	{
	protected:

		static void subscribe(const Event_Context & event_context, u32 identifier, const Delegate_Store & func);
		static void unsubscribe(const Event_Context & event_context, u32 identifier, const Delegate_Store & func);
	};

	template <typename Derived, typename Signature>
	struct Event;

	template <typename Derived, typename ... Args>
	struct Event<Derived, void(Args...)> : public Event_Base
	{
		typedef Delegate<void(Args...)> Subscriber;

		static void subscribe(Event_Context & context, const Subscriber & func);
		static void subscribe(const Event_Context & context, const Subscriber & func);
		static void subscribe(Event_Context_Owner & owner, const Subscriber & func);
		static void subscribe(const Event_Context_Owner & context, const Subscriber & func);
		static void emit(const Event_Context & context, Args && ... args);
		static void emit(const Event_Context_Owner & owner, Args && ... args);
		static void unsubscribe(Event_Context & context, const Subscriber & func);
		static void unsubscribe(const Event_Context & context, const Subscriber & func);
		static void unsubscribe(Event_Context_Owner & owner, const Subscriber & func);
		static void unsubscribe(const Event_Context_Owner & owner, const Subscriber & func);

		Event() = default;
	};

#ifdef WIN32
#ifdef VIOLET_SCRIPT
#define EVENT_API __declspec(dllimport)
#else
#define EVENT_API __declspec(dllexport)
#endif
#else
#define EVENT_API
#endif

#define DEFINE_EVENT(EventName, Signature) struct EVENT_API EventName : public vlt::Event<EventName, Signature> \
	{ \
	public: \
		static const char * get_name() { return #EventName; } \
		static u32 get_identifier() { return std::hash<std::string>()(get_name()); } \
	}

#define DEFINE_EXTERNAL_EVENT(EventName, Signature) struct EventName : public vlt::Event<EventName, Signature> \
	{ \
	public: \
		static const char * get_name() { return #EventName; } \
		static u32 get_identifier() { return std::hash<std::string>()(get_name()); } \
	}
}

#include "violet/event/event.inl"

#endif
