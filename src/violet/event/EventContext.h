#ifndef VIOLET_EventContext_H
#define VIOLET_EventContext_H

#include "violet/Defines.h"
#include "violet/utility/Delegate.h"

#include <unordered_map>
#include <vector>

namespace Violet
{
	class VIOLET_API EventContext
	{
	public:

		EventContext();
		EventContext(EventContext && other);

		void subscribe(uint32 eventId, const DelegateStore & func);
		void unsubscribe(uint32 eventId, const DelegateStore & func);

		template <typename ... Args>
		void emit(uint32 eventId, Args && ... args) const;

	private:

		EventContext(const EventContext &) = delete;
		EventContext & operator=(const EventContext &) = delete;

	private:

		std::unordered_map<uint32, std::vector<DelegateStore>> m_subscriberGroups;
	};
}

#include "violet/event/EventContext.inl"

#endif