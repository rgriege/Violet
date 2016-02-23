// ============================================================================

#include "violet/core/handle.h"

using namespace vlt;

// ============================================================================

const Handle Handle::Invalid;

// ============================================================================


Handle::Handle() :
	Handle(~0, 0)
{
}

// ----------------------------------------------------------------------------


Handle::Handle(const u16 _id, const u16 _version) :
	id(_id),
	version(_version)
{
}

// ----------------------------------------------------------------------------


bool Handle::is_valid() const
{
	return id != Invalid.id;
}

// ============================================================================


bool vlt::operator<(const Handle lhs, const Handle rhs)
{
	return lhs.id < rhs.id;
}

// ----------------------------------------------------------------------------


bool vlt::operator==(const Handle lhs, const Handle rhs)
{
	return lhs.id == rhs.id && lhs.version == rhs.version;
}

// ----------------------------------------------------------------------------


bool vlt::operator!=(const Handle lhs, const Handle rhs)
{
	return !(lhs == rhs);
}

// ============================================================================
