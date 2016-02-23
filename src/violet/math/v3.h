#ifndef VIOLET_V3_H
#define VIOLET_V3_H

#include "violet/core/defines.h"
#include "violet/core/types.h"

#include <ostream>

namespace vlt
{
	struct Deserializer;
	struct Serializer;

	struct VIOLET_API v3
	{
		r32 x, y, z;

		static const v3 X_Axis;
		static const v3 Y_Axis;
		static const v3 Z_Axis;
		static const v3 Zero;

		v3();
		v3(const v3 & rhs);
		v3(r32 _x, r32 _y, r32 _z);
		v3(Deserializer & deserializer);

		r32 magnitude() const;
		r32 mag_squared() const;
		v3 & normalize();
		v3 get_unit() const;
		bool is_unit() const;
		v3 & scale(r32 sx, r32 sy, r32 sz);
		v3 & scale(const v3 & scale);

		v3 & operator+=(const v3 & rhs);
		v3 & operator-=(const v3 & rhs);
		v3 & operator*=(r32 scale);
		v3 & operator/=(r32 scale);
		v3 & operator=(const v3 & rhs);

		r32 dot(const v3 & rhs) const;
		v3 cross(const v3 & rhs) const;
		v3 project(const v3 & axis) const;
		v3 operator-() const;
		v3 inverse() const;
		void invert();
		bool is_zero() const;
		void zero();
		v3 copy() const;
	};

	VIOLET_API v3 operator+(v3 lhs, const v3 & rhs);
	VIOLET_API v3 operator-(v3 lhs, const v3 & rhs);
	VIOLET_API v3 operator*(r32 scale, v3 vec);
	VIOLET_API v3 operator*(v3 vec, r32 scale);
	VIOLET_API v3 operator/(v3 vec, r32 scale);
	VIOLET_API bool operator==(const v3 & lhs, const v3 & rhs);
	VIOLET_API bool operator!=(const v3 & lhs, const v3 & rhs);
	VIOLET_API std::ostream & operator<<(std::ostream & os, const v3 & vec);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const v3 & vec);
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, v3 & vec);
}

#endif
