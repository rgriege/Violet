#ifndef VIOLET_Event_H
#define VIOLET_Event_H

#include "violet/event/EventContext.h"
#include "violet/event/EventContextOwner.h"
#include "violet/utility/Delegate.h"

namespace Violet
{
	class VIOLET_API EventBase
	{
	protected:

		static void subscribe(const EventContext & eventContext, uint32 identifier, const DelegateStore & func);
		static void unsubscribe(const EventContext & eventContext, uint32 identifier, const DelegateStore & func);
	};

	template <typename Derived, typename Signature>
	class Event;

	template <typename Derived, typename ... Args>
	class Event<Derived, void(Args...)> : public EventBase
	{
	public:

		typedef Delegate<void(Args...)> Subscriber;

	public:

		static void subscribe(EventContext & eventContextOwner, const Subscriber & func);
		static void subscribe(const EventContext & eventContextOwner, const Subscriber & func);
		static void subscribe(EventContextOwner & eventContext, const Subscriber & func);
		static void subscribe(const EventContextOwner & eventContext, const Subscriber & func);
		static void emit(const EventContext & eventContext, Args && ... args);
		static void emit(const EventContextOwner & eventContextOwner, Args && ... args);
		static void unsubscribe(EventContext & eventContext, const Subscriber & func);
		static void unsubscribe(const EventContext & eventContext, const Subscriber & func);
		static void unsubscribe(EventContextOwner & eventContextOwner, const Subscriber & func);
		static void unsubscribe(const EventContextOwner & eventContextOwner, const Subscriber & func);

	private:

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

#define DEFINE_EVENT(EventName, Signature) class EVENT_API EventName : public Violet::Event<EventName, Signature> \
	{ \
	public: \
		static const char * getName() { return #EventName; } \
		static uint32 getIdentifier() { return std::hash<std::string>()(getName()); } \
	}

#define DEFINE_EXTERNAL_EVENT(EventName, Signature) class EventName : public Violet::Event<EventName, Signature> \
	{ \
	public: \
		static const char * getName() { return #EventName; } \
		static uint32 getIdentifier() { return std::hash<std::string>()(getName()); } \
	}
}

#include "violet/event/Event.inl"

#endif
