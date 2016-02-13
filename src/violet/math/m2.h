#ifndef VIOLET_M2_H
#define VIOLET_M2_H

#include "violet/math/v2.h"

namespace vlt
{
	struct VIOLET_API m2
	{
		struct VIOLET_API row
		{
			r32 columns[3];

			r32 & operator[](size_t i);
			const r32 & operator[](size_t i) const;
		};

		union
		{
			row rows[3];

			struct
			{
				r32 a, b,
					c, d;
			};
		};

		static const m2 Identity;
		static const m2 Zero;

		m2();
		m2(r32 a, r32 b, r32 c, r32 d);
		explicit m2(r32 rotation);

		m2 inverse() const;
		r32 determinant() const;
		m2 & operator/=(r32 scale);
	};

	VIOLET_API v2 operator*(const m2 & mat, const v2 & vec);
	VIOLET_API m2 operator/(m2 mat, const r32 scale);
	VIOLET_API bool operator==(const m2 & lhs, const m2 & rhs);
	VIOLET_API std::ostream & operator<<(std::ostream & os, const m2 & mat);
}

#endif
