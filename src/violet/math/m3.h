#ifndef VIOLET_M3_H
#define VIOLET_M3_H

#include "violet/math/v2.h"

namespace vlt
{
	struct VIOLET_API m3
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
				r32 a, b, c,
					d, e, f,
					g, h, i;
			};
		};

		static const m3 Identity;
		static const m3 Zero;

		static m3 createRotation(r32 radians);

		row & operator[](size_t i);
		row const & operator[](size_t i) const;

		r32 * data();
		const r32 * data() const;
	};

	VIOLET_API m3 operator*(const m3 & lhs, const m3 & rhs);
	VIOLET_API v2 operator*(const m3 & lhs, const v2 & rhs);
	VIOLET_API bool operator==(const m3 & lhs, const m3 & rhs);
	VIOLET_API bool operator!=(const m3 & lhs, const m3 & rhs);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const m3 & mat);
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, m3 & mat);
}

#endif
