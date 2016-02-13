#ifndef VIOLET_M4_H
#define VIOLET_M4_H

#include <memory>

#include "violet/math/m3.h"
#include "violet/math/v3.h"
#include "violet/template/template_utilities.h"

namespace vlt
{
	struct VIOLET_API alignas(16) m4
	{
		struct VIOLET_API row
		{
			r32 columns[4];

			r32 & operator[](size_t i);
			const r32 & operator[](size_t i) const;
		};

		row rows[4];

		static const m4 Identity;
		static const m4 Zero;

		m4();
		m4(std::initializer_list<r32> values);

		row & operator[](size_t i);
		row const & operator[](size_t i) const;

		r32 * data();
		const r32 * data() const;
	};

	VIOLET_API m4 operator*(const m4 & lhs, const m4 & rhs);
	VIOLET_API v3 operator*(const m4 & lhs, const v3 & rhs);
	VIOLET_API bool operator==(const m4 & lhs, const m4 & rhs);
	VIOLET_API bool operator!=(const m4 & lhs, const m4 & rhs);
	VIOLET_API m4 from2d(const m3 & mat);
	VIOLET_API m3 to2d(const m4 & mat);
	VIOLET_API serializer & operator<<(serializer & serializer, const m4 & vec);
	VIOLET_API deserializer & operator>>(deserializer & deserializer, m4 & vec);
	
	
	struct VIOLET_API unaligned_m4
	{
		std::shared_ptr<r32> data;

		explicit unaligned_m4(const m4 & mat);
		explicit operator m4() const;
	};
	

	template <>
	struct copyable<m4>
	{
		typedef unaligned_m4 type;
	};
}

#endif
