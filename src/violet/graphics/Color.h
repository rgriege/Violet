#ifndef VIOLET_COLOR_H
#define VIOLET_COLOR_H

#include <array>

#include "violet/core/defines.h"

namespace vlt
{
	struct deserializer;
	struct serializer;

	struct VIOLET_API color final
	{	
		static const color Black;
		static const color Red;
		static const color Orange;
		static const color Yellow;
		static const color Green;
		static const color Indigo;
		static const color Blue;
		static const color Purple;
		static const color White;
		static const color Tan;
		static const color Brown;
		static const color Rainbow[];

		struct VIOLET_API component
		{
			u8 value;

			component();
			component(u8 value);
			component(r32 value);

			component & operator=(u8 value);
			component & operator=(r32 value);

			bool operator!=(u8 value) const;
			component & operator+=(u8 value);
			component & operator-=(u8 value);

			operator u8() const;
			operator r32() const;

			u8 asUint() const;
			r32 asFloat() const;
		};

		component r, g, b, a;

		color();
		color(u8 _r, u8 _g, u8 _b, u8 _a = 255);
		color(deserializer & deserializer);
		color(const char * hexString);

		bool operator!=(const color & rhs) const;

		std::array<r32, 4> as4fv() const;
	};

	deserializer & operator>>(deserializer & deserializer, color & color);
	serializer & operator<<(serializer & serializer, const color & color);
}

#endif
