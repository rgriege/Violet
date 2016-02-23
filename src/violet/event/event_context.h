#ifndef VIOLET_EVENT_CONTEXT_H
#define VIOLET_EVENT_CONTEXT_H

#include "violet/core/defines.h"
#include "violet/core/types.h"
#include "violet/utility/delegate.h"

#include <unordered_map>
#include <vector>

namespace vlt
{
	struct VIOLET_API Event_Context
	{
		std::unordered_map<u32, std::vector<Delegate_Store>> subscriber_groups;

		Event_Context();
		Event_Context(const Event_Context &) = delete;
		Event_Context & operator=(const Event_Context &) = delete;
		Event_Context(Event_Context && other);

		void subscribe(u32 event_id, const Delegate_Store & func);
		void unsubscribe(u32 event_id, const Delegate_Store & func);

		template <typename ... Args>
		void emit(u32 event_id, Args && ... args) const;
	};
}

#include "violet/event/event_context.inl"

#endif
