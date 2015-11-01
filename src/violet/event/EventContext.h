#ifndef VIOLET_EventContext_H
#define VIOLET_EventContext_H

#include "violet/Defines.h"
#include "violet/utility/FreeList.h"

#include <unordered_map>
#include <vector>

namespace Violet
{
	class VIOLET_API EventContext
	{
	private:

		struct Subscriber
		{
			Subscriber(uint32 id, void * delegate);

			uint32 m_id;
			void * m_delegate;
		};

		struct SubscriberGroup
		{
			FreeList m_idList;
			std::vector<Subscriber> m_subscribers;
		};

	public:

		EventContext();
		EventContext(EventContext && other);

		uint32 subscribe(const char * eventName, void * delegate);
		void unsubscribe(const char * eventName, uint32 delegateId);

		template <typename ... Args>
		void emit(const char * eventName, Args && ... args) const;

	private:

		EventContext(const EventContext &) = delete;
		EventContext & operator=(const EventContext &) = delete;

	private:

		std::unordered_map<const char *, SubscriberGroup> m_subscriberGroups;
	};
}

#include "violet/event/EventContext.inl"

#endif