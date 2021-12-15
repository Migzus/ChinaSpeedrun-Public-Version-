#include "SpriteRenderer.h"

#include "imgui.h"
#include "Texture.h"

cs::SpriteRenderer::SpriteRenderer() :
	zIndex{ 0 }, texture{ nullptr }
{}

void cs::SpriteRenderer::Init()
{}

void cs::SpriteRenderer::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragInt("Z Index", &zIndex);
		
		ImGui::TreePop();
	}
}

bool cs::SpriteRenderer::IsRendererValid() const
{
	return RenderComponent::IsRendererValid() && material != nullptr && texture != nullptr;
}
