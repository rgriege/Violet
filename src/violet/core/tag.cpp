// ============================================================================

#include <cstring>
#include <set>

#include "violet/core/tag.h"

using namespace vlt;

// ============================================================================

Tag::Tag(const char a, const char b, const char c, const char d) :
	characters()
{
	characters[0] = a;
	characters[1] = b;
	characters[2] = c;
	characters[3] = d;
}

// ----------------------------------------------------------------------------

Tag::Tag(const char * const str) :
	characters()
{
	strncpy(&characters[0], str, 4);
}

// ----------------------------------------------------------------------------

std::string Tag::as_string() const
{
	return std::string(&characters[0], 4);
}

// ----------------------------------------------------------------------------

bool Tag::operator<(const Tag rhs) const
{
	return value < rhs.value;
}

// ----------------------------------------------------------------------------

bool Tag::operator==(const Tag rhs) const
{
	return value == rhs.value;
}

// ----------------------------------------------------------------------------

bool Tag::operator!=(const Tag rhs) const
{
	return !(operator==(rhs));
}

// ============================================================================
