#pragma once

#include "Mathf.h"

struct ImVec4;

namespace cs
{
	class Color
	{
	public:
		float r, g, b, a;

		const static Color white;
		const static Color black;
		const static Color red;
		const static Color yellow;
		const static Color orange;
		const static Color lime;
		const static Color cyan;
		const static Color blue;
		const static Color magenta;
		const static Color transparent;

		Color();
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a);

		float* GetColorValues();
		Vector3 ConvertToGLMVector();
		Vector4 ConvertToGLMVectorAlpha();

		static ImVec4 ColorToImVec4(const Color& color);
	};
}
