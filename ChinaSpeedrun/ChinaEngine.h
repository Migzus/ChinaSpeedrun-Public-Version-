#pragma once

#include <vulkan/vulkan.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <array>

#include "Mathf.h"

struct UniformBufferObject
{
	Matrix4x4 model, view, proj;
};

struct Vertex
{
	Vector2 position;
	Vector3 color;

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};

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

class ChinaEngine
{
public:
	static std::vector<char> ReadFile(const std::string& filename);
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

	void Run();
protected:
	
private:
	const int MAX_FRAMES_IN_FLIGHT{ 2 };
	const std::string APP_NAME{ "China Speedrun" };
	const std::string ENGINE_NAME{ "ChinaEngine" };
	const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
	const bool enableValidationLayers{ false };
#else
	const bool enableValidationLayers{ true };
#endif

	const std::vector<Vertex> vertices
	{
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices
	{
		0, 1, 2, 2, 3, 0
	};

	const uint32_t WIDTH{ 800 };
	const uint32_t HEIGHT{ 600 };

	size_t currentFrame{ 0 };
	bool framebufferResized{ false };

	GLFWwindow* window;

	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkSemaphore> imageAvailableSemaphores, renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences, imagesInFlight;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkImage> swapChainImages;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	VkDescriptorPool descriptorPool;
	VkDeviceMemory vertexBufferMemory, indexBufferMemory;
	VkBuffer vertexBuffer, indexBuffer;
	VkCommandPool commandPool;
	VkPipeline graphicsPipeline;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
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
	void MainLoop();
	void DrawFrame();
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
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void CreateCommandBuffers();
	void CreateSyncObjects();

	void UpdateUniformBuffer(uint32_t currentImage);
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
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
