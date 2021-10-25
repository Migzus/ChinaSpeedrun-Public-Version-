#pragma once

#include "RenderComponent.h"

namespace cs
{
	class MeshRendererComponent : public RenderComponent
	{
	public:
		friend class MeshRenderer;
		friend class VulkanEngineRenderer;

		class Mesh* mesh;

		MeshRendererComponent();

		virtual void ImGuiDrawComponent() override;
	};

	class MeshRenderer
	{
	public:
		static void UpdateUBO(MeshRendererComponent& meshRenderer, class TransformComponent& transform);
		static void UpdateUBO(MeshRendererComponent& meshRenderer, TransformComponent& transform, class CameraComponent& camera);
		static void VulkanDraw(MeshRendererComponent& meshRenderer, VkCommandBuffer& commandBuffer, VkPipelineLayout& layout, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);
	};
}
