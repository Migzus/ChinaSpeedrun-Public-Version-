#pragma once

#include "RenderComponent.h"

namespace cs
{
	class CameraBase;
	class MeshRenderer;
	class VulkanEngineRenderer;

	class MeshRendererComponent : public RenderComponent
	{
	public:
		friend MeshRenderer;
		friend VulkanEngineRenderer;

		class Mesh* mesh;
		std::vector<class Material*> materials;

		MeshRendererComponent();

		virtual void ImGuiDrawComponent() override;
		virtual bool IsRendererValid() const override;
		virtual void VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer) override;
	
		~MeshRendererComponent();
	};

	class MeshRenderer
	{
	public:
		friend CameraBase;

		static void UpdateUBO(MeshRendererComponent& meshRenderer, class TransformComponent& transform);
		static void UpdateUBO(MeshRendererComponent& meshRenderer, TransformComponent& transform, CameraBase& camera);
		//static void VulkanDraw(MeshRendererComponent& meshRenderer, VkCommandBuffer& commandBuffer, VkPipelineLayout& layout, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);
	};
}
