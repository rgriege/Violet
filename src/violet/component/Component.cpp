// ============================================================================

#include "violet/component/Component.h"

#include <assert.h>
#include <mutex>

using namespace Violet;

// ============================================================================

uint32 Component::getNextFlag()
{
	static uint32 s_nextFlag = 1;
	static std::mutex s_mutex;

	const std::lock_guard<std::mutex> guard(s_mutex);
	const uint32 result = s_nextFlag;
	s_nextFlag <<= 1;
	assert(s_nextFlag != 0);

	return result;
}

// ============================================================================

Component::~Component()
{
}

// ============================================================================

Component::Component(const Handle entityId) :
	m_entityId(entityId)
{
}

// ----------------------------------------------------------------------------

Component::Component(Component && other) :
	m_entityId(other.m_entityId)
{
}

// ----------------------------------------------------------------------------

Handle Component::getEntityId() const
{
	return m_entityId;
}

// ============================================================================
