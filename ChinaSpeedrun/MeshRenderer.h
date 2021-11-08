#pragma once

#include "RenderComponent.h"

namespace cs
{
	class Scene;
	class CameraBase;
	class MeshRenderer;
	class VulkanEngineRenderer;

	class MeshRendererComponent : public RenderComponent
	{
	public:
		friend Scene;
		friend MeshRenderer;
		friend VulkanEngineRenderer;

		class Mesh* mesh;

		MeshRendererComponent();

		void SetMesh(Mesh* mesh);
		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
		virtual bool IsRendererValid() const override;
		virtual void VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer) override;
		
		void GenerateOBBExtents(OBB& obb);

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
