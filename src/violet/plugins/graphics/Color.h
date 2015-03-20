#ifndef COLOR_H
#define COLOR_H

#include "violet/core/Defines.h"

#include <array>

namespace Violet
{
	class Deserializer;

	class Color
	{
	public:
	
		static const Color kBlack;
		static const Color kRed;
		static const Color kOrange;
		static const Color kYellow;
		static const Color kGreen;
		static const Color kIndigo;
		static const Color kBlue;
		static const Color kPurple;
		static const Color kWhite;
		static const Color kTan;
		static const Color kBrown;
		static const Color kRainbow[];

	public:

		class Component
		{
		public:

			Component();
			Component(uint8 value);
			Component(float value);

			Component & operator=(uint8 value);
			Component & operator=(float value);

			operator uint8() const;
			operator float() const;

		private:

			uint8 m_value;
		};

	public:

		Color();
		Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255);
		Color(Deserializer & deserializer);

		std::array<float, 4> as4fv() const;

	public:

		Component r, g, b, a;
	};
}

#endif
