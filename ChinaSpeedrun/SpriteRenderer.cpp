#include "SpriteRenderer.h"

void cs::SpriteRendererComponent::Init()
{

}

void cs::SpriteRendererComponent::ImGuiDrawComponent()
{

}

bool cs::SpriteRendererComponent::IsRendererValid() const
{
	return RenderComponent::IsRendererValid() && material != nullptr && texture != nullptr;
}
