// ============================================================================

#include "violet/utility/Tag.h"

#include <set>

using namespace Violet;

// ============================================================================

Tag::Tag(char a, char b, char c, char d) :
	m_data(/*{a, b, c, d}*/)
{
	m_data[0] = a;
	m_data[1] = b;
	m_data[2] = c;
	m_data[3] = d;
}

// ----------------------------------------------------------------------------

const char * Tag::asString() const
{
	return m_data;
}

// ----------------------------------------------------------------------------

bool Tag::operator<(const Tag other) const
{
	return *reinterpret_cast<const uint32*>(m_data) < *reinterpret_cast<const uint32*>(other.m_data);
}

// ----------------------------------------------------------------------------

bool Tag::operator==(const Tag other) const
{
	return *reinterpret_cast<const uint32*>(m_data) == *reinterpret_cast<const uint32*>(other.m_data);
}

// ----------------------------------------------------------------------------

bool Tag::operator!=(const Tag other) const
{
	return !(operator==(other));
}

// ============================================================================
