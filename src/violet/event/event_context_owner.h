#ifndef VIOLET_EVENT_CONTEXT_OWNER_H
#define VIOLET_EVENT_CONTEXT_OWNER_H

#include "violet/event/event_context.h"

namespace vlt
{
	struct VIOLET_API event_context_owner
	{
		event_context event_context;

		event_context_owner();
		event_context_owner(const event_context_owner &) = delete;
		event_context_owner & operator=(const event_context_owner &) = delete;
		event_context_owner(event_context_owner && other);
	};
}

#endif