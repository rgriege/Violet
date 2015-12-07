#ifndef VIOLET_Event_H
#define VIOLET_Event_H

#include "violet/event/EventContext.h"
#include "violet/event/EventContextOwner.h"

#include <functional>

namespace Violet
{
	class VIOLET_API EventBase
	{
	protected:

		static void unsubscribe(const EventContext & eventContext, uint32 identifier, uint32 delegateId);
	};

	template <typename Derived, typename Signature>
	class Event;

	template <typename Derived, typename ... Args>
	class Event<Derived, void(Args...)> : public EventBase
	{
	public:

		typedef std::function<void(Args...)> Delegate;

	private:

		struct StoredDelegate
		{
			uint32 m_id;
			Delegate * m_method;
		};

	public:

		static uint32 subscribe(EventContext & eventContextOwner, const Delegate & func);
		static uint32 subscribe(EventContextOwner & eventContext, const Delegate & func);
		static void emit(const EventContext & eventContext, Args && ... args);
		static void emit(const EventContextOwner & eventContextOwner, Args && ... args);
		static void unsubscribe(EventContext & eventContext, uint32 delegateId);
		static void unsubscribe(const EventContext & eventContext, uint32 delegateId);
		static void unsubscribe(EventContextOwner & eventContextOwner, uint32 delegateId);
		static void unsubscribe(const EventContextOwner & eventContextOwner, uint32 delegateId);

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
