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
		class Material* material;

		virtual void ImGuiDrawComponent() override;
		virtual bool IsRendererValid() const override;
	};

	class SpriteRenderer : public Renderer
	{
	public:

	};
}
