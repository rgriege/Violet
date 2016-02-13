#ifndef VIOLET_EVENT_CONTEXT_H
#define VIOLET_EVENT_CONTEXT_H

#include "violet/core/defines.h"
#include "violet/core/types.h"
#include "violet/utility/delegate.h"

#include <unordered_map>
#include <vector>

namespace vlt
{
	struct VIOLET_API event_context
	{
	public:

		event_context();
		event_context(event_context && other);

		void subscribe(u32 event_id, const delegate_store & func);
		void unsubscribe(u32 event_id, const delegate_store & func);

		template <typename ... Args>
		void emit(u32 event_id, Args && ... args) const;

	private:

		event_context(const event_context &) = delete;
		event_context & operator=(const event_context &) = delete;

	private:

		std::unordered_map<u32, std::vector<delegate_store>> subscriber_groups;
	};
}

#include "violet/event/event_context.inl"

#endif