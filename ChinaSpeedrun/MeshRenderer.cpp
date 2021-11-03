#include "MeshRenderer.h"

#include "VulkanEngineRenderer.h"
#include "Transform.h"
#include "Mesh.h"
#include "ChinaEngine.h"
#include "World.h"
#include "CameraComponent.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"

void cs::MeshRenderer::UpdateUBO(MeshRendererComponent& meshRenderer, TransformComponent& transform)
{
	meshRenderer.ubo.model = transform;
}

void cs::MeshRenderer::UpdateUBO(MeshRendererComponent& meshRenderer, TransformComponent& transform, CameraBase& camera)
{
	meshRenderer.ubo.model = transform;
	meshRenderer.ubo.proj = Camera::GetProjectionMatrix(camera);
	meshRenderer.ubo.view = Camera::GetViewMatrix(camera);
}

cs::MeshRendererComponent::MeshRendererComponent() :
	mesh{ nullptr }
{
	ChinaEngine::renderer.SolveRenderer(this, Solve::ADD);
	ChinaEngine::renderer.AddToRenderQueue(this);
	// perhaps move this line to the vulkan renderer...
	uboOffset = ChinaEngine::world.GetUBONextOffset();
}

void cs::MeshRendererComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool _visible{ visible };
		ImGui::Checkbox("Visible", &_visible);
		SetVisible(_visible);

		ImGui::TreePop();
	}
}

bool cs::MeshRendererComponent::IsRendererValid() const
{
	return RenderComponent::IsRendererValid() && mesh != nullptr && !materials.empty();
}

void cs::MeshRendererComponent::VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer)
{
	if (!IsRendererValid())
		return;

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, materials[0]->pipeline);

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &mesh->vertexBufferOffset);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, mesh->indexBufferOffset, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, materials[0]->shader->layout, 0, 1, &descriptorSets[index], 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->GetIndices().size()), 1, 0, 0, 0);
}

cs::MeshRendererComponent::~MeshRendererComponent()
{
	ChinaEngine::renderer.RemoveFromRenderQueue(this);
}
