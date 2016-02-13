// ============================================================================

#include <cstring>
#include <set>

#include "violet/core/tag.h"

using namespace vlt;

// ============================================================================

tag::tag(const char a, const char b, const char c, const char d) :
	data({a, b, c, d})
{
}

// ----------------------------------------------------------------------------

tag::tag(const char * const str) :
	data()
{
	strncpy(&data[0], str, 4);
}

// ----------------------------------------------------------------------------

std::string tag::as_string() const
{
	return std::string(&data[0], 4);
}

// ----------------------------------------------------------------------------

bool tag::operator<(const tag rhs) const
{
	return *reinterpret_cast<const u32*>(as_string().c_str()) < *reinterpret_cast<const u32*>(rhs.as_string().c_str());
}

// ----------------------------------------------------------------------------

bool tag::operator==(const tag rhs) const
{
	return *reinterpret_cast<const u32*>(as_string().c_str()) == *reinterpret_cast<const u32*>(rhs.as_string().c_str());
}

// ----------------------------------------------------------------------------

bool tag::operator!=(const tag rhs) const
{
	return !(operator==(rhs));
}

// ============================================================================
