#include "MeshRenderer.h"

#include "VulkanEngineRenderer.h"
#include "Transform.h"
#include "Mesh.h"
#include "ChinaEngine.h"
#include "World.h"
#include "CameraComponent.h"
#include "Camera.h"

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

void cs::MeshRenderer::VulkanDraw(MeshRendererComponent& meshRenderer, VkCommandBuffer& commandBuffer, VkPipelineLayout& layout, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer)
{
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &meshRenderer.mesh->vertexBufferOffset);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, meshRenderer.mesh->indexBufferOffset, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &meshRenderer.descriptorSets[index], 0, nullptr);

	// the current mesh index, with its indices goes down below
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(meshRenderer.mesh->GetIndices().size()), 1, 0, 0, 0);
}

cs::MeshRendererComponent::MeshRendererComponent() :
	mesh{ nullptr }
{
	ChinaEngine::renderer.SolveRenderer(this, Solve::ADD);
	// perhaps move this line to the vulkan renderer...
	uboOffset = ChinaEngine::world.GetUBONextOffset();
}

void cs::MeshRendererComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{


		ImGui::TreePop();
	}
}

bool cs::MeshRendererComponent::IsRendererValid() const
{
	return RenderComponent::IsRendererValid() && mesh != nullptr && !materials.empty();
}
