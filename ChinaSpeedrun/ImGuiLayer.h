#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct ImVec2;

namespace cs
{
	class VulkanEngineRenderer;
	class GameObject;

	namespace editor
	{
		class ImGuiLayer
		{
		public:
			friend VulkanEngineRenderer;

			class EngineEditor* editorRoot;
			GameObject* activeObject;

			ImGuiLayer(EngineEditor* root);

			void Init();
			void Begin();
			void Step();
			void End();
			void SetStyle();

			const bool& IsManipulating() const;
			const bool& IsInteractingWithWindow() const;

		private:
			bool isManipulating;
			bool isWindowActive;

			VkRenderPass renderPass;
			VkDescriptorPool descriptorPool;
			VkCommandPool commandPool;
			std::vector<VkCommandBuffer> commandBuffers;
			std::vector<VkFramebuffer> framebuffers;

			void DrawStopSimulationButton();
			void IsWindowHovered();
		};
	}
}
