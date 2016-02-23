#ifndef VIOLET_V2_H
#define VIOLET_V2_H

#include "violet/core/defines.h"
#include "violet/core/types.h"

#include <ostream>

namespace vlt
{
	struct Deserializer;
	struct Serializer;

	struct VIOLET_API v2
	{
		union
		{
			struct
			{
				r32 x, y;
			};

			struct
			{
				r32 u, v;
			};
		};

		static const v2 X_Axis;
		static const v2 Y_Axis;
		static const v2 Zero;

	public:

		v2();
		v2(const v2 & rhs);
		v2(r32 _x, r32 _y);
		v2(Deserializer & deserializer);

		r32 magnitude() const;
		r32 mag_squared() const;
		v2 & normalize();
		v2 get_unit() const;
		bool is_unit() const;
		v2 & scale(r32 scaleX, r32 scaleY);
		v2 & scale(const v2 & scale);

		v2 & operator+=(const v2 & other);
		v2 & operator-=(const v2 & other);
		v2 & operator*=(r32 scale);
		v2 & operator/=(r32 scale);
		v2 & operator=(const v2 & other);

		void rotate(r32 radians);
		r32 dot(const v2 & other) const;
		r32 cross(const v2 & other) const;
		v2 project(const v2 & axis) const;
		v2 perpendicular(bool left = true) const;
		v2 operator-() const;
		v2 inverse() const;
		void invert();
		bool is_zero() const;
		void zero();
		bool shares_quadrant(const v2 & other) const;
		v2 copy() const;
	};

	VIOLET_API v2 operator+(v2 lhs, const v2 & rhs);
	VIOLET_API v2 operator-(v2 lhs, const v2 & rhs);
	VIOLET_API v2 operator*(r32 scale, v2 vec);
	VIOLET_API v2 operator*(v2 vec, r32 scale);
	VIOLET_API v2 operator/(v2 vec, r32 scale);
	VIOLET_API bool operator==(const v2 & lhs, const v2 & rhs);
	VIOLET_API bool operator!=(const v2 & lhs, const v2 & rhs);
	VIOLET_API std::ostream & operator<<(std::ostream & os, const v2 & vec);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const v2 & v);
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, v2 & v);
}

#endif
