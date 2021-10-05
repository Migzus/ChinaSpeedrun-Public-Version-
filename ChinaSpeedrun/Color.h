#pragma once

namespace cs
{
	class Color
	{
	public:
		const static Color white;
		const static Color black;
		const static Color red;
		const static Color yellow;
		const static Color lime;
		const static Color cyan;
		const static Color blue;
		const static Color magenta;
		const static Color transparent;

		float r, g, b, a;

		Color();
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a);

		float* GetColorValues();
	};
}
