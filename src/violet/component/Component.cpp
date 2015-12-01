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

Component::Component(Entity & owner) :
	m_owner(owner)
{
}

// ----------------------------------------------------------------------------

Component::Component(Component && other) :
	m_owner(other.m_owner)
{
}

// ----------------------------------------------------------------------------

Entity & Component::getOwner()
{
	return m_owner;
}

// ----------------------------------------------------------------------------

const Entity & Component::getOwner() const
{
	return m_owner;
}

// ============================================================================
