#pragma once

/// <summary>
/// 
/// in the mesh renderer, we have to store information about the different descriptor offsets
/// because, multiple objects can have the same shader, the same material, the same mesh
/// Essentially, they can be duplicates, BUT they can have different orientations
/// 
/// </summary>

#include "Component.h"

#include <vulkan/vulkan.h>
#include <vector>

// this will inherit from Component (because an object can have a mesh that renders on screen)
namespace cs
{
	class MeshRenderer : public Component
	{
	public:
		bool active;								 // TEMP 
		std::vector<class Material*> materials;
		class Mesh* mesh;
		struct UniformBufferObject* ubo;             // TEMP
		VkDeviceSize uboOffset;                      // TEMP
		VkDescriptorPool descriptorPool;             // TEMP
		std::vector<VkDescriptorSet> descriptorSets; // TEMP

		MeshRenderer();
		MeshRenderer(Mesh* newMesh);

		void Update(size_t index);
		uint16_t GetUBOSize() const;

		const std::vector<VkDescriptorSet>& GetDescriptorSet() const;

		~MeshRenderer();

	private:
	};
}
