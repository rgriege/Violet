#include "violet/plugins/graphics/Color.h"

using namespace Violet;

const Color Color::kBlack(0, 0, 0);
const Color Color::kRed(1, 0, 0);
const Color Color::kOrange(1, 0.50f, 0);
const Color Color::kYellow(1, 1, 0);
const Color Color::kGreen(0, 1, 0);
const Color Color::kIndigo(0, 1, 1);
const Color Color::kBlue(0, 0, 1);
const Color Color::kPurple(0.55f, 0, 1);
const Color Color::kWhite(1, 1, 1);
const Color Color::kTan(0.94f, 0.90f, 0.66f);
const Color Color::kBrown(0.42f, 0.26f, 0.15f);
const Color Color::kRainbow[] = {kRed, kOrange, kYellow, kGreen, kIndigo, kBlue, kPurple};

Color::Color() :
	r(0),
	g(0),
	b(0)
{
}

Color::Color(const float _r, const float _g, const float _b) :
	r(_r),
	g(_g),
	b(_b)
{
}