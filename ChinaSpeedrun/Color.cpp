#include "Color.h"

#include "imgui.h"

const cs::Color cs::Color::white{ 1.0f, 1.0f, 1.0f, 1.0f };
const cs::Color cs::Color::black{ 0.0f, 0.0f, 0.0f, 1.0f };
const cs::Color cs::Color::red{ 1.0f, 0.0f, 0.0f, 1.0f };
const cs::Color cs::Color::yellow{ 1.0f, 1.0f, 0.0f, 1.0f };
const cs::Color cs::Color::orange{ 1.0f, 0.37f, 0.0f, 1.0f };
const cs::Color cs::Color::lime{ 0.0f, 1.0f, 0.0f, 1.0f };
const cs::Color cs::Color::cyan{ 0.0f, 1.0f, 1.0f, 1.0f };
const cs::Color cs::Color::blue{ 0.0f, 0.0f, 1.0f, 1.0f };
const cs::Color cs::Color::magenta{ 1.0f, 0.0f, 1.0f, 1.0f };
const cs::Color cs::Color::transparent{ 0.0f, 0.0f, 0.0f, 0.0f };

cs::Color::Color() :
	r{ 1.0f }, g{ 1.0f }, b{ 1.0f }, a{ 1.0f }
{}

cs::Color::Color(float r, float g, float b) :
	r{ r }, g{ g }, b{ b }, a{ 1.0f }
{}

cs::Color::Color(float r, float g, float b, float a) :
	r{ r }, g{ g }, b{ b }, a{ a }
{}

float* cs::Color::GetColorValues()
{
	return &r;
}

Vector3 cs::Color::ConvertToGLMVector()
{
	return { r, g, b };
}

Vector4 cs::Color::ConvertToGLMVectorAlpha()
{
	return { r, g, b, a };
}

ImVec4 cs::Color::ColorToImVec4(const Color& color)
{
	return ImVec4(color.r, color.g, color.b, color.a);
}
