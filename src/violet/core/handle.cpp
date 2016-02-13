// ============================================================================

#include "violet/core/handle.h"

using namespace vlt;

// ============================================================================


const handle handle::Invalid;

// ============================================================================


handle::handle() :
	handle(~0, 0)
{
}

// ----------------------------------------------------------------------------


handle::handle(const u16 id, const u16 version) :
	id(id),
	version(version)
{
}

// ----------------------------------------------------------------------------


bool handle::is_valid() const
{
	return id != Invalid.id;
}

// ============================================================================


bool vlt::operator<(const handle lhs, const handle rhs)
{
	return lhs.id < rhs.id;
}

// ----------------------------------------------------------------------------


bool vlt::operator==(const handle lhs, const handle rhs)
{
	return lhs.id == rhs.id && lhs.version == rhs.version;
}

// ----------------------------------------------------------------------------


bool vlt::operator!=(const handle lhs, const handle rhs)
{
	return !(lhs == rhs);
}

// ============================================================================
