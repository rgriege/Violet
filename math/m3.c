// ============================================================================

#include <cmath>

#include "violet/math/m3.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

r32 & m3::row::operator[](const size_t i)
{
	return columns[i];
}

// ----------------------------------------------------------------------------

const r32 & m3::row::operator[](const size_t i) const
{
	return columns[i];
}

// ============================================================================

const m3 m3::Identity = {
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
};

// ----------------------------------------------------------------------------

const m3 m3::Zero = {
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f
};

// ============================================================================

m3 m3::createRotation(const r32 radians)
{
	return{
		std::cos(radians), -std::sin(radians), 0.f,
		std::sin(radians), std::cos(radians), 0.f,
		0.f, 0.f, 1.f
	};
}

// ============================================================================

typename m3::row & m3::operator[](const size_t i)
{
	return rows[i];
}

// ----------------------------------------------------------------------------

typename m3::row const & m3::operator[](const size_t i) const
{
	return rows[i];
}

// ----------------------------------------------------------------------------

r32 * m3::data()
{
	return &rows[0][0];
}

// ----------------------------------------------------------------------------

const r32 * m3::data() const
{
	return &rows[0][0];
}

// ============================================================================

m3 vlt::operator*(const m3 & lhs, const m3 & rhs)
{
	m3 result = m3::Zero;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int h = 0; h < 3; h++)
				result[i][j] += lhs[i][h] * rhs[h][j];
	return result;
}

// ----------------------------------------------------------------------------

v2 vlt::operator*(const m3 & lhs, const v2 & rhs)
{
	return v2(lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2], lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2]);
}

// ----------------------------------------------------------------------------

bool vlt::operator==(const m3 & lhs, const m3 & rhs)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (lhs[i][j] != rhs[i][j])
				return false;
	return true;
}

// ----------------------------------------------------------------------------

bool vlt::operator!=(const m3 & lhs, const m3 & rhs)
{
	return !(lhs == rhs);
}

// ============================================================================

Serializer & vlt::operator<<(Serializer & serializer, const m3 & mat)
{
	auto segment = serializer.create_segment("mat");
	segment->write_r32("a", mat[0][0]);
	segment->write_r32("b", mat[0][1]);
	segment->write_r32("c", mat[0][2]);
	segment->write_r32("d", mat[1][0]);
	segment->write_r32("e", mat[1][1]);
	segment->write_r32("f", mat[1][2]);
	segment->write_r32("g", mat[2][0]);
	segment->write_r32("h", mat[2][1]);
	segment->write_r32("i", mat[2][2]);
	return serializer;
}

// ----------------------------------------------------------------------------

Deserializer & vlt::operator>>(Deserializer & deserializer, m3 & mat)
{
	auto segment = deserializer.enter_segment("mat");
	mat[0][0] = segment->get_r32("a");
	mat[0][1] = segment->get_r32("b");
	mat[0][2] = segment->get_r32("c");
	mat[1][0] = segment->get_r32("d");
	mat[1][1] = segment->get_r32("e");
	mat[1][2] = segment->get_r32("f");
	mat[2][0] = segment->get_r32("g");
	mat[2][1] = segment->get_r32("h");
	mat[2][2] = segment->get_r32("i");
	return deserializer;
}

// ============================================================================
