#pragma once

#include "RenderComponent.h"

namespace cs
{
	// This component is still a work in progress
	class SpriteRendererComponent : public RenderComponent
	{
	public:
		friend class SpriteRenderer;

		class Texture* texture;

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
		virtual bool IsRendererValid() const override;
	};

	class SpriteRenderer : public Renderer
	{
	public:

	};
}
