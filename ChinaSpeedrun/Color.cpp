#include "Color.h"

using namespace cs;

const Color Color::white{ 1.0f, 1.0f, 1.0f, 1.0f };
const Color Color::black{ 0.0f, 0.0f, 0.0f, 1.0f };
const Color Color::red{ 1.0f, 0.0f, 0.0f, 1.0f };
const Color Color::yellow{ 1.0f, 1.0f, 0.0f, 1.0f };
const Color Color::lime{ 0.0f, 1.0f, 0.0f, 1.0f };
const Color Color::cyan{ 0.0f, 1.0f, 1.0f, 1.0f };
const Color Color::blue{ 0.0f, 0.0f, 1.0f, 1.0f };
const Color Color::magenta{ 1.0f, 0.0f, 1.0f, 1.0f };
const Color Color::transparent{ 0.0f, 0.0f, 0.0f, 0.0f };

Color::Color() :
	r{ 1.0f }, g{ 1.0f }, b{ 1.0f }, a{ 1.0f }
{}

Color::Color(float r, float g, float b) :
	r{ r }, g{ g }, b{ b }, a{ 1.0f }
{}

Color::Color(float r, float g, float b, float a) :
	r{ r }, g{ g }, b{ b }, a{ a }
{}
