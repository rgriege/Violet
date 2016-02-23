#ifndef VIOLET_COLOR_H
#define VIOLET_COLOR_H

#include <array>

#include "violet/core/defines.h"

namespace vlt
{
	struct Deserializer;
	struct Serializer;

	struct VIOLET_API Color final
	{	
		static const Color Black;
		static const Color Red;
		static const Color Orange;
		static const Color Yellow;
		static const Color Green;
		static const Color Indigo;
		static const Color Blue;
		static const Color Purple;
		static const Color White;
		static const Color Tan;
		static const Color Brown;
		static const Color Rainbow[];

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

			u8 as_uint() const;
			r32 as_float() const;
		};

		component r, g, b, a;

		Color();
		Color(u8 r, u8 g, u8 b, u8 a = 255);
		Color(Deserializer & deserializer);
		Color(const char * hex_string);

		bool operator!=(const Color & rhs) const;

		std::array<r32, 4> as4fv() const;
	};

	Deserializer & operator>>(Deserializer & deserializer, Color & color);
	Serializer & operator<<(Serializer & serializer, const Color & color);
}

#endif
