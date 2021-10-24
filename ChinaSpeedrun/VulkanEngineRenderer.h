#pragma once

#include "ChinaCoreHeader.h"

#ifndef NDEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

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

	struct VulkanBufferInfo
	{
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		VkDeviceSize bufferSize, dataSize;

		VulkanBufferInfo(VkDeviceSize newBufferSize);
	};

	enum class Solve
	{
		NONE,
		UPDATE,
		ADD,
		REMOVE
	};

	class VulkanEngineRenderer
	{
	public:
		VulkanEngineRenderer();

		constexpr static unsigned int MAX_BUFFER_SIZE{ UINT32_MAX / 16 };

		static void FramebufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight);

		// Called before rendering happens, this is to ensure we have resolved all
		// materials, shaders and gameobjects to prepare them for rendering.
		void Resolve();

		void SolveMesh(class Mesh* mesh, Solve solveMode);
		void SolveShader(class Shader* shader, Solve solveMode);
		void SolveTexture(class Texture* texture, Solve solveMode);
		void SolveMaterial(class Material* material, Solve solveMode);
		void SolveRenderer(class MeshRendererComponent* renderer, Solve solveMode);

		void Create(int newWidth, int newHeight, const char* appTitle);
		void GetViewportSize(int& widthRef, int& heightRef) const;
		void DrawFrame();
		VkDevice const& GetDevice();

		float AspectRatio() const;

		GLFWwindow* GetWindow();

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

		// This member can be moved into the shader class
		VkSampleCountFlagBits msaaSamples{ VK_SAMPLE_COUNT_1_BIT };
		size_t currentFrame{ 0 };
		bool framebufferResized{ false };

		GLFWwindow* window;
		int width, height;
		std::string appName;

		std::unordered_map<Mesh*, Solve> solveMeshes;
		std::unordered_map<Shader*, Solve> solveShaders;
		std::unordered_map<Texture*, Solve> solveTextures;
		std::unordered_map<Material*, Solve> solveMaterials;
		std::unordered_map<MeshRendererComponent*, Solve> solveRenderers;

		std::vector<VkSemaphore> imageAvailableSemaphores, renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences, imagesInFlight;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkImage> swapChainImages;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		VulkanBufferInfo vertexBuffer, indexBuffer;

		// Spesific to ImGui Stuff
		VkRenderPass imGuiRenderPass;
		VkDescriptorPool imGuiDescriptorPool;
		VkCommandPool imGuiCommandPool;
		std::vector<VkCommandBuffer> imGuiCommandBuffers;
		std::vector<VkFramebuffer> imGuiFramebuffers;

		VkImage colorImage;
		VkDeviceMemory colorImageMemory;
		VkImageView colorImageView;

		VkImageView textureImageView, depthImageView;
		VkSampler textureSampler;
		uint32_t mipLevels;
		VkImage textureImage, depthImage;
		VkDeviceMemory textureImageMemory, depthImageMemory;
		VkCommandPool commandPool;
		//VkDescriptorPool descriptorPool;
		//VkPipeline graphicsPipeline;
		VkRenderPass renderPass;
		//VkPipelineLayout pipelineLayout;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		VkSwapchainKHR swapChain;
		VkSurfaceKHR surface;
		VkQueue graphicsQueue, presentQueue;
		VkDevice device;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

		void AllocateMesh(Mesh* mesh);
		void AllocateTexture(Texture* texture);
		void AllocateShader(Shader* shader);
		void AllocateMaterial(Material* material);

		void UpdateShader(Shader* shader);
		void UpdateTexture(Texture* texture);
		void UpdateMaterial(Material* material);

		void FreeMesh(Mesh* mesh);
		void FreeTexture(Texture* texture);
		void FreeShader(Shader* shader);
		void FreeMaterial(Material* shader);

		void InitWindow();
		void InitVulkan();
		void InitImGui();
		void Cleanup();

		void ImGuiRenderPass();
		void ImGuiFramebuffers();
		void ImGuiDescriptorPool();
		void ImGuiCommandPool();
		void ImGuiCommandBuffers();

		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDescriptorSetLayout(Shader* shader);
		void CreateGraphicsPipeline(Material* material);
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateColorResources();
		void CreateDepthResources();
		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateUniformBuffers();
		void CreateDescriptorPool(MeshRendererComponent& renderer);
		void CreateDescriptorSets(MeshRendererComponent& renderer);
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void CreateImage(uint32_t width, uint32_t height, uint32_t mipmapLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipmapLevels);
		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		VkSampleCountFlagBits GetMaxSampleCount();
		bool HasStencilComponent(VkFormat format);
		VkFormat FindDepthFormat();
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		void UpdateUniformBuffer(uint32_t currentImage);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		VkCommandBuffer BeginSingleTimeCommands();
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipmapLevels);
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
