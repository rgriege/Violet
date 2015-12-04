// ============================================================================

#include "violet/handle/Handle.h"

using namespace Violet;

// ============================================================================

const Handle Handle::ms_invalid;

// ============================================================================

Handle::Handle() :
    Handle(~0, ~0)
{
}

// ----------------------------------------------------------------------------

Handle::Handle(const uint32 id, const uint32 version) :
	m_id(id),
	m_version(version)
{
}

// ----------------------------------------------------------------------------

uint32 Handle::getId() const
{
	return m_id;
}

// ----------------------------------------------------------------------------

uint32 Handle::getVersion() const
{
	return m_version;
}

// ----------------------------------------------------------------------------

bool Handle::isValid() const
{
	return *this != Handle::ms_invalid;
}

// ============================================================================

bool Violet::operator<(const Handle lhs, const Handle rhs)
{
	return lhs.getId() < rhs.getId();
}

// ----------------------------------------------------------------------------

bool Violet::operator==(Handle lhs, Handle rhs)
{
	return lhs.getId() == rhs.getId() && lhs.getVersion() == rhs.getVersion();
}

// ----------------------------------------------------------------------------

bool Violet::operator!=(Handle lhs, Handle rhs)
{
	return !(lhs == rhs);
}

// ============================================================================
