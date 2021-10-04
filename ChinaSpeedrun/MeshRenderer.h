#pragma once

#include "RenderComponent.h"

namespace cs
{
	class MeshRendererComponent
	{
	public:
		friend class MeshRenderer;
		friend class VulkanEngineRenderer;

		std::vector<class Material*> materials;
		class Mesh* mesh;

	private:
		UniformBufferObject ubo;
		VkDeviceSize uboOffset;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
	};

	class MeshRenderer
	{
	public:
		static void VulkanDraw(MeshRendererComponent& meshRenderer, VkCommandBuffer& commandBuffer, VkPipelineLayout& layout, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);
	};
}
