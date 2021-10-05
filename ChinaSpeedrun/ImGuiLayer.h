#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class ImGuiLayer
{
public:
	void Init();
	void Begin();
	void End();
	void SetStyle();

private:
	VkRenderPass renderPass;
	VkDescriptorPool descriptorPool;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> framebuffers;
};
