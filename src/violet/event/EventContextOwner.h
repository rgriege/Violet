#ifndef VIOLET_EventContextOwner_H
#define VIOLET_EventContextOwner_H

#include "violet/event/EventContext.h"

namespace Violet
{
	class VIOLET_API EventContextOwner
	{
	public:

		EventContextOwner();
		EventContextOwner(EventContextOwner && other);

		EventContext & getEventContext();
		const EventContext & getEventContext() const;

	private:

		EventContextOwner(const EventContextOwner &) = delete;
		EventContextOwner & operator=(const EventContextOwner &) = delete;

	private:

		EventContext m_eventContext;
	};
}

#endif