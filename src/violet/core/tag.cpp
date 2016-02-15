// ============================================================================

#include <cstring>
#include <set>

#include "violet/core/tag.h"

using namespace vlt;

// ============================================================================

tag::tag(const char a, const char b, const char c, const char d) :
	characters({a, b, c, d})
{
}

// ----------------------------------------------------------------------------

tag::tag(const char * const str) :
	characters()
{
	strncpy(&characters[0], str, 4);
}

// ----------------------------------------------------------------------------

std::string tag::as_string() const
{
	return std::string(&characters[0], 4);
}

// ----------------------------------------------------------------------------

bool tag::operator<(const tag rhs) const
{
	return value < rhs.value;
}

// ----------------------------------------------------------------------------

bool tag::operator==(const tag rhs) const
{
	return value == rhs.value;
}

// ----------------------------------------------------------------------------

bool tag::operator!=(const tag rhs) const
{
	return !(operator==(rhs));
}

// ============================================================================
