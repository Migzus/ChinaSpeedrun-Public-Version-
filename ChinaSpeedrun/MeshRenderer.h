#pragma once

/// <summary>
/// 
/// in the mesh renderer, we have to store information about the different descriptor offsets
/// because, multiple objects can have the same shader, the same material, the same mesh
/// Essentially, they can be duplicates, BUT they can have different orientations
/// 
/// </summary>

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

// this will inherit from Component (because an object can have a mesh that renders on screen)
namespace cs
{
	class MeshRendererComponent
	{

	};

	class MeshRenderer
	{
	public:
		std::string name;							 // TEMP
		bool active;								 // TEMP 
		std::vector<class Material*> materials;
		class Mesh* mesh;
		struct UniformBufferObject* ubo;             // TEMP
		VkDeviceSize uboOffset;                      // TEMP
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;             // TEMP
		std::vector<VkDescriptorSet> descriptorSets; // TEMP

		MeshRenderer();
		MeshRenderer(Mesh* newMesh);

		void Update(size_t index);
		void VulkanDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout& layout, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);
		uint16_t GetUBOSize() const;

		const std::vector<VkDescriptorSet>& GetDescriptorSet() const;

		~MeshRenderer();

	private:
	};
}
