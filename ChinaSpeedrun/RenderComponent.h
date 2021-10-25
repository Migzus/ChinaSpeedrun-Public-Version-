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
		//friend class SpriteRenderer;
		friend class MeshRenderer;
		friend class VulkanEngineRenderer;

		std::vector<class Material*> materials;

		virtual void ImGuiDrawComponent() override;

	protected:
		UniformBufferObject ubo;
		VkDeviceSize uboOffset;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
	};

	class Renderer
	{
	public:

	};
}
