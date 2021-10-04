#pragma once

#include "Mathf.h"

#include <vulkan/vulkan.h>
#include <vector>

// Will fill this in the future
// MeshRenderer and SpriteRenderer will inherit from this
namespace cs
{
	struct UniformBufferObject
	{
		Matrix4x4 model, view, proj;

		UniformBufferObject();

		static uint32_t GetByteSize();
	};

	class RenderComponent
	{
	public:
		std::vector<class Material*> materials;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
	};

	class Renderer
	{
	public:

	};
}
