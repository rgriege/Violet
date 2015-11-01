#ifndef VIOLET_Event_H
#define VIOLET_Event_H

#include "violet/event/EventContext.h"

#include <functional>

namespace Violet
{
	template <typename Derived, typename Signature>
	class Event;

	template <typename Derived, typename ... Args>
	class Event<Derived, void(Args...)>
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

		static uint32 subscribe(EventContext & eventContext, const Delegate & func);
		static void emit(const EventContext & eventContext, Args && ... args);
		static void unsubscribe(EventContext & eventContext, uint32 delegateId);

	private:

		Event() = default;
	};

#ifdef WIN32
#ifdef VIOLET_SCRIPT
#define SCRIPT_API __declspec(dllimport)
#else
#define SCRIPT_API __declspec(dllexport)
#endif
#else
#define SCRIPT_API
#endif

#define DEFINE_EVENT(EventName, Signature) class SCRIPT_API EventName : public Violet::Event<EventName, Signature> \
	{ \
	public: \
		static const char * getName() { return #EventName; } \
	}
}

#include "violet/event/Event.inl"

#endif