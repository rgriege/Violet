// ============================================================================

#include "violet/event/EventContextOwner.h"

using namespace Violet;

// ============================================================================

EventContextOwner::EventContextOwner() :
	m_eventContext()
{
}

// ----------------------------------------------------------------------------

EventContextOwner::EventContextOwner(EventContextOwner && other) :
	m_eventContext(std::move(other.m_eventContext))
{
}

// ----------------------------------------------------------------------------

EventContext & EventContextOwner::getEventContext()
{
	return m_eventContext;
}

// ----------------------------------------------------------------------------

const EventContext & EventContextOwner::getEventContext() const
{
	return m_eventContext;
}

// ============================================================================
