#ifndef COLOR_H
#define COLOR_H

namespace Violet
{
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

		Color();
		Color(float _r, float _g, float _b);

	public:

		const float r, g, b;
	};
}

#endif
