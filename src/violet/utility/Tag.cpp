// ============================================================================

#include "violet/utility/Tag.h"

#include <set>

using namespace Violet;

// ============================================================================

Tag::Tag(const char a, const char b, const char c, const char d) :
	m_data({a, b, c, d})
{
}

// ----------------------------------------------------------------------------

Tag::Tag(const char * const str) :
	m_data()
{
	strncpy(&m_data[0], str, 4);
}

// ----------------------------------------------------------------------------

std::string Tag::asString() const
{
	return std::string(&m_data[0], 4);
}

// ----------------------------------------------------------------------------

bool Tag::operator<(const Tag other) const
{
	return *reinterpret_cast<const uint32*>(asString().c_str()) < *reinterpret_cast<const uint32*>(other.asString().c_str());
}

// ----------------------------------------------------------------------------

bool Tag::operator==(const Tag other) const
{
	return *reinterpret_cast<const uint32*>(asString().c_str()) == *reinterpret_cast<const uint32*>(other.asString().c_str());
}

// ----------------------------------------------------------------------------

bool Tag::operator!=(const Tag other) const
{
	return !(operator==(other));
}

// ============================================================================
