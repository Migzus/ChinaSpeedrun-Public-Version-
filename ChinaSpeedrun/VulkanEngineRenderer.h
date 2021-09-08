#pragma once

#include "ChinaCoreHeader.h"

namespace cs
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete();
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanEngineRenderer
	{
	public:
		static void FramebufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight);

		void AllocateMesh(class Mesh* mesh);
		void AllocateTexture(class Texture* texture);
		void AllocateShader(class Shader* shader);

		void FreeMesh(Mesh* mesh);
		void FreeTexture(Texture* mesh);
		void FreeShader(Shader* mesh);

		void Create(int newWidth, int newHeight, const char* appTitle);
		void GetViewportSize(int& widthRef, int& heightRef) const;
		void DrawFrame();

		float AspectRatio() const;

		GLFWwindow* GetWindow();
		VkDevice& GetDevice();

		~VulkanEngineRenderer();

	private:
		const int MAX_FRAMES_IN_FLIGHT{ 2 };
		const std::string ENGINE_NAME{ "China Engine" };
		const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
		const bool enableValidationLayers{ false };
#else
		const bool enableValidationLayers{ true };
#endif

		size_t currentFrame{ 0 };
		bool framebufferResized{ false };

		GLFWwindow* window;
		int width, height;
		std::string appName;

		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkSemaphore> imageAvailableSemaphores, renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences, imagesInFlight;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkImage> swapChainImages;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		VkImageView textureImageView, depthImageView;
		VkSampler textureSampler;
		VkImage textureImage, depthImage;
		VkDeviceMemory vertexBufferMemory, indexBufferMemory, textureImageMemory, depthImageMemory;
		VkBuffer vertexBuffer, indexBuffer;
		VkCommandPool commandPool;
		VkPipeline graphicsPipeline;
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		VkSwapchainKHR swapChain;
		VkSurfaceKHR surface;
		VkQueue graphicsQueue, presentQueue;
		VkDevice device;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

		void InitWindow();
		void InitVulkan();
		void Cleanup();

		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateDepthResources();
		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		bool HasStencilComponent(VkFormat format);
		VkFormat FindDepthFormat();
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		void UpdateUniformBuffer(uint32_t currentImage);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		VkCommandBuffer BeginSingleTimeCommands();
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void RecreateSwapChain();
		void CleanupSwapChain();

		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		int RateDeviceSuitability(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		bool CheckValidationSupport();
		std::vector<const char*> GetRequiredExtensions();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	};
}
