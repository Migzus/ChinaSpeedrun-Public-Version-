#pragma once

#include "RenderComponent.h"

namespace cs
{
	/**
	* Renders a 2D quad with a texture, also called a sprite.
	*/
	class SpriteRenderer : public RenderComponent
	{
	public:
		int32_t zIndex;
		class Texture* texture;

		SpriteRenderer();

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
		virtual bool IsRendererValid() const override;
	};
}
