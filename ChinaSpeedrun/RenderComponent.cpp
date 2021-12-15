#include "RenderComponent.h"

#include "imgui.h"
#include "GameObject.h"
#include "Scene.h"

cs::UniformBufferObject::UniformBufferObject() :
	model{ Matrix4x4(1.0f) }, view{ Matrix4x4(1.0f) }, proj{ Matrix4x4(1.0f) }
{}

uint32_t cs::UniformBufferObject::GetByteSize()
{
	return sizeof(UniformBufferObject);
}

cs::RenderComponent::RenderComponent() :
	uboOffset{ sizeof(float) }
{}

void cs::RenderComponent::Init()
{}

void cs::RenderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TreePop();
	}
}

bool cs::RenderComponent::IsRendererValid() const
{
	return descriptorPool != nullptr && !descriptorSets.empty() && visible;
}

void cs::RenderComponent::VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer)
{}

void cs::RenderComponent::SetVisible(const bool status)
{
	if (status != visible)
	{
		visible = status;

		if (visible)
			gameObject->GetScene()->AddToRenderQueue(this);
		else
			gameObject->GetScene()->RemoveFromRenderQueue(this);
	}
}
