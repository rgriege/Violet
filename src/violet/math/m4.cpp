// ============================================================================

#include <assert.h>
#include <cstring>
#include <nmmintrin.h>

#include "violet/math/m4.h"

#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

r32 & m4::row::operator[](const size_t i)
{
	return columns[i];
}

// ----------------------------------------------------------------------------


const r32 & m4::row::operator[](const size_t i) const
{
	return columns[i];
}

// ============================================================================

const m4 m4::Identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

// ----------------------------------------------------------------------------

const m4 m4::Zero = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

// ============================================================================

m4::m4() :
	rows()
{
	std::memset(data(), 0, 16 * sizeof(r32));
}

// ----------------------------------------------------------------------------

m4::m4(std::initializer_list<r32> values) :
	rows()
{
	assert(values.size() == 16);
	std::memcpy(data(), values.begin(), 16 * sizeof(r32));
}

// ----------------------------------------------------------------------------

m4::row & m4::operator[](const size_t i)
{
	return rows[i];
}

// ----------------------------------------------------------------------------

const m4::row & m4::operator[](const size_t i) const
{
	return rows[i];
}

// ----------------------------------------------------------------------------

r32 * m4::data()
{
	return &rows[0][0];
}

// ----------------------------------------------------------------------------

const r32 * m4::data() const
{
	return &rows[0][0];
}

// ============================================================================

m4 vlt::operator*(const m4 & lhs, const m4 & rhs)
{
	m4 result = m4::Zero;
	for (int i = 0; i < 4; ++i)
	{
		for (int k = 0; k < 4; ++k)
		{
			r32 r = lhs[i][k];
			__m128 rVec = _mm_set_ps(r, r, r, r);

			__m128 rhsVec = _mm_load_ps(&rhs[k][0]);
			__m128 resultVec = _mm_load_ps(&result[i][0]);
			__m128 mul = _mm_mul_ps(rVec, rhsVec);
			__m128 add = _mm_add_ps(resultVec, mul);
			_mm_store_ps(&result[i][0], add);
		}
	}

	return result;
}

// ----------------------------------------------------------------------------

v3 vlt::operator*(const m4 & lhs, const v3 & rhs)
{
	return v3(lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2], lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2], lhs[2][0] * rhs.x + lhs[2][1] * rhs.y + lhs[2][2] * rhs.z);
}

// ----------------------------------------------------------------------------

bool vlt::operator==(const m4 & lhs, const m4 & rhs)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (lhs[i][j] != rhs[i][j])
				return false;
	return true;
}

// ----------------------------------------------------------------------------

bool vlt::operator!=(const m4 & lhs, const m4 & rhs)
{
	return !(lhs == rhs);
}

// ----------------------------------------------------------------------------

m4 vlt::from2d(const m3 & mat)
{
	return m4 {
		mat[0][0], mat[0][1], 0, mat[0][2],
			mat[1][0], mat[1][1], 0, mat[1][2],
			0, 0, 1, 0,
			mat[2][0], mat[2][1], 0, mat[2][2]
	};
}

// ----------------------------------------------------------------------------

m3 vlt::to2d(const m4 & mat)
{
	return m3 {
		mat[0][0], mat[0][1], mat[0][3],
			mat[1][0], mat[1][1], mat[1][3],
			mat[3][0], mat[3][1], mat[3][3]
	};
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const m4 & mat)
{
	auto segment = serializer.create_segment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			segment->write_r32(label.c_str(), mat[i][j]);
			++label[0];
		}
	}
	return serializer;
}

// ----------------------------------------------------------------------------

deserializer & vlt::operator>>(deserializer & deserializer, m4 & mat)
{
	auto segment = deserializer.enter_segment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat[i][j] = segment->get_r32(label.c_str());
			++label[0];
		}
	}
	return deserializer;
}

// ============================================================================

unaligned_m4::unaligned_m4(const m4 & mat) :
	data(new r32[16], std::default_delete<r32[]>())
{
	memcpy(data.get(), mat.data(), 16 * sizeof(r32));
}

// ----------------------------------------------------------------------------


unaligned_m4::operator m4() const
{
	m4 result;
	memcpy(result.data(), data.get(), 16 * sizeof(r32));
	return result;
}

// ============================================================================
