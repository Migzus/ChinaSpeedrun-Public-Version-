#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct ImVec2;

namespace cs
{
	class ImGuiLayer
	{
	public:
		void Init();
		void Begin();
		void End();
		void SetStyle();

		bool BeginButtonDropDown(const char* label, ImVec2 buttonSize);
		void EndButtonDropDown();

	private:
		VkRenderPass renderPass;
		VkDescriptorPool descriptorPool;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkFramebuffer> framebuffers;
	};
}
