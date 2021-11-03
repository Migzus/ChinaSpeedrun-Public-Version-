#pragma once

#include "Mathf.h"
#include "Component.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace cs
{
	struct UniformBufferObject
	{
		Matrix4x4 model, view, proj;

		UniformBufferObject();

		static uint32_t GetByteSize();
	};

	class RenderComponent : public Component
	{
	public:
		friend class Renderer;
		friend class MeshRenderer;
		friend class VulkanEngineRenderer;

		bool visible;

		virtual void ImGuiDrawComponent() override;
		virtual bool IsRendererValid() const;
		virtual void VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);
		virtual void SetVisible(const bool status);

	protected:
		UniformBufferObject ubo;
		VkDeviceSize uboOffset;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
		
		bool dirtyUpdatedUBO;
	};

	class Renderer
	{
	public:
	};
}
