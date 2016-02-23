#ifndef VIOLET_EVENT_CONTEXT_OWNER_H
#define VIOLET_EVENT_CONTEXT_OWNER_H

#include "violet/event/event_context.h"

namespace vlt
{
	struct VIOLET_API Event_Context_Owner
	{
		Event_Context event_context;

		Event_Context_Owner();
		Event_Context_Owner(const Event_Context_Owner &) = delete;
		Event_Context_Owner & operator=(const Event_Context_Owner &) = delete;
		Event_Context_Owner(Event_Context_Owner && other);
	};
}

#endif
