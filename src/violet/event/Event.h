#ifndef VIOLET_EVENT_H
#define VIOLET_EVENT_H

#include "violet/event/event_context.h"
#include "violet/event/event_context_owner.h"
#include "violet/utility/delegate.h"

namespace vlt
{
	struct VIOLET_API event_base
	{
	protected:

		static void subscribe(const event_context & event_context, u32 identifier, const delegate_store & func);
		static void unsubscribe(const event_context & event_context, u32 identifier, const delegate_store & func);
	};

	template <typename Derived, typename Signature>
	struct event;

	template <typename Derived, typename ... Args>
	struct event<Derived, void(Args...)> : public event_base
	{
	public:

		typedef delegate<void(Args...)> subscriber;

	public:

		static void subscribe(event_context & event_context_owner, const subscriber & func);
		static void subscribe(const event_context & event_context_owner, const subscriber & func);
		static void subscribe(event_context_owner & event_context, const subscriber & func);
		static void subscribe(const event_context_owner & event_context, const subscriber & func);
		static void emit(const event_context & event_context, Args && ... args);
		static void emit(const event_context_owner & event_context_owner, Args && ... args);
		static void unsubscribe(event_context & event_context, const subscriber & func);
		static void unsubscribe(const event_context & event_context, const subscriber & func);
		static void unsubscribe(event_context_owner & event_context_owner, const subscriber & func);
		static void unsubscribe(const event_context_owner & event_context_owner, const subscriber & func);

	private:

		event() = default;
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

#define DEFINE_EVENT(EventName, Signature) struct EVENT_API EventName : public vlt::event<EventName, Signature> \
	{ \
	public: \
		static const char * get_name() { return #EventName; } \
		static u32 get_identifier() { return std::hash<std::string>()(get_name()); } \
	}

#define DEFINE_EXTERNAL_EVENT(EventName, Signature) struct EventName : public vlt::event<EventName, Signature> \
	{ \
	public: \
		static const char * get_name() { return #EventName; } \
		static u32 get_identifier() { return std::hash<std::string>()(get_name()); } \
	}
}

#include "violet/event/event.inl"

#endif
