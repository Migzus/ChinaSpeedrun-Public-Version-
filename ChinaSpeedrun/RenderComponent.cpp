#include "RenderComponent.h"

#include "imgui.h"

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
	return descriptorPool != nullptr && !descriptorSets.empty();
}
