#include "RenderComponent.h"

#include "imgui.h"
#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"

cs::UniformBufferObject::UniformBufferObject() :
	model{ Matrix4x4(1.0f) }, view{ Matrix4x4(1.0f) }, proj{ Matrix4x4(1.0f) }
{}

uint32_t cs::UniformBufferObject::GetByteSize()
{
	return sizeof(UniformBufferObject);
}

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
			ChinaEngine::renderer.AddToRenderQueue(this);
		else
			ChinaEngine::renderer.RemoveFromRenderQueue(this);
	}
}
