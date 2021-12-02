#pragma once

#include "Color.h"
#include "Draw.h"
#include "Component.h"

namespace cs
{
	class DirectionalLight : public Component
	{
	public:
		float intensity;
		Color lightColor;

		DirectionalLight();

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
		void MakeRayLines();

	private:
		DrawItem item;
	};
}
