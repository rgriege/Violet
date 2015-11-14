// ============================================================================

#include "violet/log/LogTarget.h"

#include "violet/log/Log.h"

using namespace Violet;

// ============================================================================

LogTarget::Guard::Guard(const uint32 handle) :
	m_handle(handle)
{
}

// ----------------------------------------------------------------------------

LogTarget::Guard::~Guard()
{
	Log::removeTarget(m_handle);
}

// ============================================================================
